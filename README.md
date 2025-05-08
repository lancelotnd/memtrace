# memtrace

This utility leverages LTTNG-ust to trace memory events on heterogeneoussystems (CPU/GPU) and memory movement/copies between host and device. While not limited to it, it is meant as a diagnostic tool to understand if memory is handled properly on architecture with unified-memory such as HPC nodes equipped with AMD MI300A. In such systems memory copies between host and device become redundant and programs should implement zero-copy. 

## How it works 
This tool works by using LD_PRELOAD to override hip functions related to memory in order to add custom tracepoints that will report to LTTng-UST. This way any program can be traced without requiring further intrumentation.

## Requirements
- A linux Operating System with ROCm installed
- LTTng UST installed
- ctf2ctf to convert CommonTraceFormat into Json.

All the dependencies can be installed in user-mode as this is meant to work on HPC shared node. As such any installation can be done using `make install` instead of the typical `sudo make install`. These programs will therefore be in `/usr/local/bin` and libs under `/usr/local/lib`

## Usage
Compile the hipwrapper.cpp into a library and stores it into `/usr/local/wrap_hip.so` or any preferred directory

```sh
g++ -fPIC -shared -o /usr/local/wrap_hip.so \
    wrap10.cpp hiptrace.c \
    -ldl -llttng-ust -rdynamic \
    -I. -I/opt/rocm/include \
    -L/usr/local/lib -Wl,-rpath,/usr/local/lib \
    -D__HIP_PLATFORM_AMD__
```

```sh
LTTNG_UST_CTL_PATH=/home/users/lancend/mestraces lttng-sessiond --daemonize --no-kernel --group=prl_collab -v
lttng create May05-trace
lttng enable-channel --userspace --blocking-timeout=inf blocking-channel
lttng enable-event -c blocking-channel -u lttng_ust_libc* 
lttng enable-event -c blocking-channel -u lttng_ust_statedump*  
lttng enable-event -c blocking-channel -u hiptrace*
lttng add-context -c blocking-channel -u -t vpid -t vtid
lttng start
LTTNG_UST_ALLOW_BLOCKING=1 LTTNG_UST_APP_PATH="/home/users/lancend/mestraces" LTTNG_UST_DEBUG=1 LTTNG_UST_VERBOSE=1 LD_PRELOAD="/usr/local/lib/liblttng-ust-libc-wrapper.so:/home/users/lancend/code/wrap_hip.so" <YOUR_EXECUTABLE>
```

To export the CTF trace into json call `ctf2ctf <TRACE_DIRECTORY> > trace.json`. 