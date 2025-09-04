# filepath: [lttng-start.sh](http://_vscodecontentref_/0)
#!/usr/bin/env bash
set -euo pipefail

conda activate unidev
mydate=$(date +"%b%d-%H%M" | tr '[:upper:]' '[:lower:]')

export LTTNG_HOME="$HOME/mestraces"                # sessiond base dir
export LTTNG_UST_APP_PATH="$LTTNG_HOME/.lttng"     # apps socket dir
mkdir -p "$LTTNG_UST_APP_PATH"

pkill -u "$USER" -f lttng-sessiond || true
# Start sessiond manually to be explicit (inherits env)
lttng-sessiond -d

lttng destroy || true
lttng create "$mydate"
cd ..
lttng enable-channel --userspace --blocking-timeout=inf blocking-channel
lttng enable-event -c blocking-channel -u "hiptrace:*"
cd -
lttng add-context -c blocking-channel -u -t vpid -t vtid
lttng start

cd ~/pytorch
LTTNG_UST_ALLOW_BLOCKING=1 LTTNG_UST_APP_PATH="/home/users/lancend/mestraces" LTTNG_UST_DEBUG=1 LTTNG_UST_VERBOSE=1 LD_PRELOAD="/usr/local/lib/liblttng-ust-libc-wrapper.so:/usr/local/wrap_hip.so" python ~/f5.py --model llama --train_batch_size=1 --eval_batch_size 1 --num_train_samples=1 --name test1
lttng stop
ctf2ctf ~/lttng-traces/$mydate* --output ~/code/S25-Experiments/memtrace/traces/$mydate.json 2>/dev/null

#python ~/code/S25-Experiments/memtrace/process_trace.py ~/code/S25-Experiments/memtrace/traces/$mydate_tmp.json ~/code/S25-Experiments/memtrace/traces/$mydate.json

