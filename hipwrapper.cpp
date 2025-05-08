#include <hip/hip_runtime_api.h>
#include <stdio.h>
#include <dlfcn.h>
#include "hiptrace.h"


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

    hipError_t result = real(ptr, size);
    if (result == hipSuccess) {
        tracepoint(hiptrace, hip_malloc, size, *ptr, result);
    } else {
        tracepoint(hiptrace, hip_malloc, size, nullptr, result);
    }
    return result;
}

extern "C" hipError_t hipMallocManaged(void** ptr, size_t size, unsigned int flags) {
    using Fn = hipError_t (*)(void**, size_t, unsigned int);
    static Fn real = load_symbol<Fn>("hipMallocManaged");

    if (!real) return hipErrorUnknown;

    hipError_t result = real(ptr, size, flags);
    if (result == hipSuccess) {
        tracepoint(hiptrace, hip_malloc_managed, size, *ptr, flags, result);
    } else {
        tracepoint(hiptrace, hip_malloc_managed, size, nullptr, flags, result);
    }
    return result;
}

extern "C" hipError_t hipMemcpyAsync(void* dst, const void* src, size_t size, hipMemcpyKind kind, hipStream_t stream) {
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind, hipStream_t);
    static Fn real = load_symbol<Fn>("hipMemcpyAsync");

    if (!real) return hipErrorUnknown;

    // Create a new id 
    // Get the current ns

    hipError_t result = real(dst, src, size, kind, stream);

    // create a stupid event that will fire the moment the 
    tracepoint(hiptrace, hip_memcpy_async, dst, src, size, kind, stream, result);
    return result;
}

extern "C" hipError_t hipMemcpyWithStream(void* dst, const void* src, size_t size, hipMemcpyKind kind, hipStream_t stream) {
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind, hipStream_t);
    static Fn real = load_symbol<Fn>("hipMemcpyWithStream");

    if (!real) return hipErrorUnknown;

    hipError_t result = real(dst, src, size, kind, stream);
    tracepoint(hiptrace, hip_memcpy_with_stream, dst, src, size, kind, stream, result);
    return result;
}

extern "C" hipError_t hipMemcpy(void* dst, const void* src, size_t size, hipMemcpyKind kind) {
    using Fn = hipError_t (*)(void*, const void*, size_t, hipMemcpyKind);
    static Fn real = load_symbol<Fn>("hipMemcpy");

    if (!real) return hipErrorUnknown;

    hipError_t result = real(dst, src, size, kind);
    tracepoint(hiptrace, hip_memcpy, dst, src, size, kind, result);
    return result;
}

extern "C" hipError_t hipFree(void* ptr) {
    using Fn = hipError_t (*)(void*);
    static Fn real = load_symbol<Fn>("hipFree");

    if (!real) return hipErrorUnknown;

    hipError_t result = real(ptr);
    tracepoint(hiptrace, hip_free, ptr, result);
    return result;
}
