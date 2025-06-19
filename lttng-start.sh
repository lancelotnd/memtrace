mydate=$(date +"%b%d-%H%M" | tr '[:upper:]' '[:lower:]')
lttng destroy
lttng create $mydate
lttng enable-channel --userspace --blocking-timeout=inf blocking-channel
cd ../
lttng enable-event -c blocking-channel -u hiptrace*
cd -
lttng add-context -c blocking-channel -u -t vpid -t vtid
lttng start
LTTNG_UST_ALLOW_BLOCKING=1 LTTNG_UST_APP_PATH="/home/users/lancend/mestraces" LTTNG_UST_DEBUG=1 LTTNG_UST_VERBOSE=1 LD_PRELOAD="/usr/local/lib/liblttng-ust-libc-wrapper.so:/usr/local/wrap_hip.so" ./hip_async_test
lttng stop
lttng view