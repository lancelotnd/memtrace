#include <hip/hip_runtime_api.h>
#include <stdio.h>
#include <dlfcn.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include "hiptrace.h"


static inline uint64_t next_id(){
    static std::atomic<uint64_t> g{1};
    return g.fetch_add(1, std::memory_order_relaxed);
}

struct CopyCtx {
    uint64_t id;
    size_t size;
    hipMemcpyKind kind;
    hipEvent_t start;
    const void* src;
    void* dst;
};
static std::unordered_map<hipEvent_t, CopyCtx> g_map;
static std::mutex g_map_mtx;

// Helpers to get device/memory info
static inline int current_device() {
    int d = -1;
    if (hipGetDevice(&d) != hipSuccess) return -1;
    return d;
}

static inline int pointer_device(const void* p, hipMemoryType* memTypeOut = nullptr) {
    if (!p) {
        if (memTypeOut) *memTypeOut = hipMemoryTypeHost;
        return -1;
    }
    int dev = -1;
    int mtInt = (int)hipMemoryTypeHost;
    // Query device ordinal and memory type via attribute API (works across HIP versions)
    (void)hipPointerGetAttribute(&dev, HIP_POINTER_ATTRIBUTE_DEVICE_ORDINAL, const_cast<void*>(p));
    (void)hipPointerGetAttribute(&mtInt, HIP_POINTER_ATTRIBUTE_MEMORY_TYPE, const_cast<void*>(p));
    if (memTypeOut) *memTypeOut = static_cast<hipMemoryType>(mtInt);
    return dev;
}


template<typename FuncType>
FuncType load_symbol(const char* name) {
    static void* handle = dlopen("libamdhip64.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return nullptr;
    }
    void* sym = dlsym(handle, name);
    if (!sym) {
        fprintf(stderr, "dlsym failed for %s: %s\n", name, dlerror());
        return nullptr;
    }
    return reinterpret_cast<FuncType>(sym);
}

extern "C" hipError_t hipMalloc(void** ptr, size_t size) {
    using Fn = hipError_t (*)(void**, size_t);
    static Fn real = load_symbol<Fn>("hipMalloc");

    if (!real) return hipErrorUnknown;
    // At entry, ptr is not yet set; also record current device ordinal
    const int cur_dev = current_device();
    tracepoint(hiptrace, hip_malloc_entry, size, nullptr, 0, cur_dev);
    hipError_t result = real(ptr, size);
    if (result == hipSuccess) {
        const int owner_dev = pointer_device(*ptr);
        tracepoint(hiptrace, hip_malloc_exit, size, *ptr, result, owner_dev);
    } else {
        const int owner_dev = -1;
        tracepoint(hiptrace, hip_malloc_exit, size, nullptr, result, owner_dev);
    }
    return result;
}

extern "C" hipError_t hipMallocManaged(void** ptr, size_t size, unsigned int flags) {
    using Fn = hipError_t (*)(void**, size_t, unsigned int);
    static Fn real = load_symbol<Fn>("hipMallocManaged");

    if (!real) return hipErrorUnknown;

    const int cur_dev = current_device();
    hipError_t result = real(ptr, size, flags);
    if (result == hipSuccess) {
        const int owner_dev = pointer_device(*ptr);
        tracepoint(hiptrace, hip_malloc_managed, size, *ptr, flags, result, cur_dev, owner_dev);
    } else {
        const int owner_dev = -1;
        tracepoint(hiptrace, hip_malloc_managed, size, nullptr, flags, result, cur_dev, owner_dev);
    }
    return result;
}

extern "C" hipError_t hipMemcpyAsync(void* dst, const void* src, size_t size, hipMemcpyKind kind, hipStream_t stream) {
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind, hipStream_t);
    using Fn2 = hipError_t (*)(hipEvent_t*, unsigned int);
    using Fn3 = hipError_t (*)(hipEvent_t, hipStream_t);
    using Fn4 = hipError_t (*)(hipStream_t, hipStreamCallback_t, void*, unsigned int);
    // hipEventElapsedTime
    using Fn5 = hipError_t (*)(float*, hipEvent_t, hipEvent_t);
    //EventDestroy
    using Fn6 = hipError_t (*)(hipEvent_t);
    using Fn7 = hipError_t (*)(hipStream_t);
    
    static Fn real =         load_symbol<Fn>("hipMemcpyAsync");
    static Fn2 eventCreate = load_symbol<Fn2>("hipEventCreateWithFlags");
    static Fn3 eventRecord = load_symbol<Fn3>("hipEventRecord");
    static Fn4 streamAddCallback = load_symbol<Fn4>("hipStreamAddCallback");
    static Fn5 eventElapsedTime = load_symbol<Fn5>("hipEventElapsedTime");
    static Fn6 eventDestroy = load_symbol<Fn6>("hipEventDestroy");
    static Fn7 streamSynchronize = load_symbol<Fn7>("hipStreamSynchronize");

    if (!real) return hipErrorUnknown;

    // Create a new id 
    // Get the current ns

    hipEvent_t ev_start, ev_stop;
    eventCreate(&ev_start, hipEventDefault);
    eventCreate(&ev_stop, hipEventDefault);
    //We write the start event on the same stream right before the copy
    eventRecord(ev_start, stream);
    // We queue the copy
    hipError_t result = real(dst, src, size, kind, stream); //async

    //Record the stop event after the copy in the same stream.
    eventRecord(ev_stop,stream);

    //to remember the context
    uint64_t id = next_id();
    {
        std::lock_guard lk(g_map_mtx);
        g_map.emplace(ev_stop,CopyCtx{id, size, kind, ev_start, src, dst});
    }

      // -------- host callback when stream reaches ev_stop ----------
      streamAddCallback(stream,
        [](hipStream_t, hipError_t status, void* user) {
            hipEvent_t ev_stop = (hipEvent_t)user;
            CopyCtx ctx;
            {
                std::lock_guard lk(g_map_mtx);
                ctx = g_map[ev_stop];
                g_map.erase(ev_stop);
            }

            // GPU times: begin = 0 by default; we get only elapsed (μs)
            float ms = 0.f;
            eventElapsedTime(&ms, ctx.start, ev_stop);   // GPU clock
            uint64_t dur_ns = static_cast<uint64_t>(ms * 1e6);

            // Hip gives elapsed, not absolute; we only need Δ
            hipMemoryType srcType{}, dstType{};
            int srcDev = pointer_device(ctx.src, &srcType);
            int dstDev = pointer_device(ctx.dst, &dstType);
            tracepoint(hiptrace, hip_memcpy_async_span,
                       ctx.id, ctx.size, ctx.kind, ctx.src, ctx.dst,
                       dur_ns /*end == begin+dur*/, srcDev, dstDev, (int)srcType, (int)dstType);

            eventDestroy(ctx.start);
            eventDestroy(ev_stop);
        },
        ev_stop,
        0 /*flags*/);
    return result;
}


extern "C" hipError_t hipMemcpyWithStream(void* dst, const void* src, size_t size, hipMemcpyKind kind, hipStream_t stream) {
    //When the default stream is used, this is a blocking call.
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind, hipStream_t);
    static Fn real = load_symbol<Fn>("hipMemcpyWithStream");

    if (!real) return hipErrorUnknown;

    hipMemoryType srcType{}, dstType{};
    int srcDev = pointer_device(src, &srcType);
    int dstDev = pointer_device(dst, &dstType);
    tracepoint(hiptrace, hip_memcpy_with_stream_entry, dst, src, size, kind, stream, 0,
               srcDev, dstDev, (int)srcType, (int)dstType);
    hipError_t result = real(dst, src, size, kind, stream);
    tracepoint(hiptrace, hip_memcpy_with_stream_exit, dst, src, size, kind, stream, result,
               srcDev, dstDev, (int)srcType, (int)dstType);
    return result;
}

extern "C" hipError_t hipMemcpy(void* dst, const void* src, size_t size, hipMemcpyKind kind) {
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind);
    static Fn real = load_symbol<Fn>("hipMemcpy");

    if (!real) return hipErrorUnknown;
    hipMemoryType srcType{}, dstType{};
    int srcDev = pointer_device(src, &srcType);
    int dstDev = pointer_device(dst, &dstType);
    tracepoint(hiptrace, hip_memcpy_entry, dst, src, size, kind, 0,
               srcDev, dstDev, (int)srcType, (int)dstType);
    hipError_t result = real(dst, src, size, kind);
    tracepoint(hiptrace, hip_memcpy_exit, dst, src, size, kind, result,
               srcDev, dstDev, (int)srcType, (int)dstType);
    return result;
}

extern "C" hipError_t hipFree(void* ptr) {
    using Fn = hipError_t (*)(void*);
    static Fn real = load_symbol<Fn>("hipFree");

    if (!real) return hipErrorUnknown;
    // capture owner device/type before free
    const int owner_dev = pointer_device(ptr);
    tracepoint(hiptrace, hip_free_entry, ptr, 0, owner_dev);
    hipError_t result = real(ptr);
    tracepoint(hiptrace, hip_free_exit, ptr, result, owner_dev);
    return result;
}
