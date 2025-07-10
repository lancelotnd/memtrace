conda activate unidev
mydate=$(date +"%b%d-%H%M" | tr '[:upper:]' '[:lower:]')
lttng destroy
lttng create $mydate
lttng enable-channel --userspace --blocking-timeout=inf blocking-channel
cd ../
lttng enable-event -c blocking-channel -u lttng_ust_libc:malloc
lttng enable-event -c blocking-channel -u lttng_ust_libc:calloc
lttng enable-event -c blocking-channel -u lttng_ust_libc:free
lttng enable-event -c blocking-channel -u lttng_ust_libc:realloc 
lttng enable-event -c blocking-channel -u hiptrace*
cd -
lttng add-context -c blocking-channel -u -t vpid -t vtid
lttng start
cd ~/pytorch
LTTNG_UST_ALLOW_BLOCKING=1 LTTNG_UST_APP_PATH="/home/users/lancend/mestraces" LTTNG_UST_DEBUG=1 LTTNG_UST_VERBOSE=1 LD_PRELOAD="/usr/local/lib/liblttng-ust-libc-wrapper.so:/usr/local/wrap_hip.so" python ~/f5.py --model llama --train_batch_size=1 --eval_batch_size 1 --num_train_samples=1 --name test1
lttng stop
ctf2ctf ~/lttng-traces/$mydate* --output ~/code/S25-Experiments/memtrace/traces/$mydate.json 2>/dev/null

