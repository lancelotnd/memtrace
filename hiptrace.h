
#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER hiptrace

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "./hiptrace.h"

#if !defined(HIPTRACE_H) || defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define HIPTRACE_H

#include <lttng/tracepoint.h>

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_malloc,
    LTTNG_UST_TP_ARGS(
        size_t, size,
        void*, ptr,
        int, result
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_malloc_managed,
    LTTNG_UST_TP_ARGS(
        size_t, size,
        void*, ptr,
        unsigned int, flags,
        int, result
    ),
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(unsigned int, flags, flags)
        lttng_ust_field_integer(int, result, result)
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
    hip_memcpy_with_stream,
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
    hip_memcpy_async_span,
    LTTNG_UST_TP_ARGS(uint64_t, id,
            size_t,   size,
            int,      kind,
            const void*, src,
            void*,    dst,
            uint64_t, duration_ns),
    TP_FIELDS(
        lttng_ust_field_integer(uint64_t, id, id)
        lttng_ust_field_integer(size_t,   size, size)
        lttng_ust_field_integer(int,      kind, kind)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer(uint64_t, duration_ns,   duration_ns)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_memcpy,
    LTTNG_UST_TP_ARGS(
        void*, dst,
        const void*, src,
        size_t, size,
        int, kind,
        int, result
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, dst, dst)
        lttng_ust_field_integer_hex(const void*, src, src)
        lttng_ust_field_integer(size_t, size, size)
        lttng_ust_field_integer(int, kind, kind)
        lttng_ust_field_integer(int, result, result)
    )
)

LTTNG_UST_TRACEPOINT_EVENT(
    hiptrace,
    hip_free,
    LTTNG_UST_TP_ARGS(
        void*, ptr,
        int, result
    ),
    TP_FIELDS(
        lttng_ust_field_integer_hex(void*, ptr, ptr)
        lttng_ust_field_integer(int, result, result)
    )
)


#endif /* HIPTRACE_H */

#include <lttng/tracepoint-event.h>
