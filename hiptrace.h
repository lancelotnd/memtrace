
#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER hiptrace

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "./hiptrace.h"

#if !defined(HIPTRACE_H) || defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define HIPTRACE_H

#include <lttng/tracepoint.h>


LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_malloc_entry,
    LTTNG_UST_TP_ARGS(
        size_t, size,
        void*, ptr,
    int, result,
    int, cur_dev
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, cur_dev, cur_dev)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_malloc_exit,
    LTTNG_UST_TP_ARGS(
        size_t, size,
        void*, ptr,
    int, result,
    int, owner_dev
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, owner_dev, owner_dev)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_malloc_managed,
    LTTNG_UST_TP_ARGS(
        size_t, size,
        void*, ptr,
        unsigned int, flags,
    int, result,
    int, cur_dev,
    int, owner_dev
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(unsigned int, flags, flags)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, cur_dev, cur_dev)
    lttng_ust_field_integer(int, owner_dev, owner_dev)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_async,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
        void*, stream,
        int, result
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer_hex(void*, stream, stream)
        lttng_ust_field_integer(int, result, result)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_with_stream_entry,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
        void*, stream,
    int, result,
    int, src_dev,
    int, dst_dev,
    int, src_type,
    int, dst_type
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer_hex(void*, stream, stream)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, src_dev, src_dev)
    lttng_ust_field_integer(int, dst_dev, dst_dev)
    lttng_ust_field_integer(int, src_type, src_type)
    lttng_ust_field_integer(int, dst_type, dst_type)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_with_stream_exit,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
        void*, stream,
    int, result,
    int, src_dev,
    int, dst_dev,
    int, src_type,
    int, dst_type
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer_hex(void*, stream, stream)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, src_dev, src_dev)
    lttng_ust_field_integer(int, dst_dev, dst_dev)
    lttng_ust_field_integer(int, src_type, src_type)
    lttng_ust_field_integer(int, dst_type, dst_type)
    )
)


LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_async_span,
    LTTNG_UST_TP_ARGS(uint64_t, id,
            size_t,   size,
            int,      kind,
            const void*, src,
            void*,    dst,
            uint64_t, duration_ns,
            int,      src_dev,
            int,      dst_dev,
            int,      src_type,
            int,      dst_type),
    TP_FIELDS(
        lttng_ust_field_integer(uint64_t, id, id)
        lttng_ust_field_integer(size_t,   size, size)
        lttng_ust_field_integer(int,      kind, kind)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer(uint64_t, duration_ns,   duration_ns)
        lttng_ust_field_integer(int,      src_dev, src_dev)
        lttng_ust_field_integer(int,      dst_dev, dst_dev)
        lttng_ust_field_integer(int,      src_type, src_type)
        lttng_ust_field_integer(int,      dst_type, dst_type)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_entry,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
    int, result,
    int, src_dev,
    int, dst_dev,
    int, src_type,
    int, dst_type
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, src_dev, src_dev)
    lttng_ust_field_integer(int, dst_dev, dst_dev)
    lttng_ust_field_integer(int, src_type, src_type)
    lttng_ust_field_integer(int, dst_type, dst_type)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy_exit,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
    int, result,
    int, src_dev,
    int, dst_dev,
    int, src_type,
    int, dst_type
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, src_dev, src_dev)
    lttng_ust_field_integer(int, dst_dev, dst_dev)
    lttng_ust_field_integer(int, src_type, src_type)
    lttng_ust_field_integer(int, dst_type, dst_type)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_free_entry,
    LTTNG_UST_TP_ARGS(
        void*, ptr,
    int, result,
    int, owner_dev
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, owner_dev, owner_dev)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_free_exit,
    LTTNG_UST_TP_ARGS(
        void*, ptr,
    int, result,
    int, owner_dev
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    lttng_ust_field_integer(int, owner_dev, owner_dev)
    )
)


#endif /* HIPTRACE_H */

#include <lttng/tracepoint-event.h>
