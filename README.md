# memtrace

This utility leverages LTTNG-ust to trace memory events on heterogeneoussystems (CPU/GPU) and memory movement/copies between host and device. While not limited to it, it is meant as a diagnostic tool to understand if memory is handled properly on architecture with unified-memory such as HPC nodes equipped with AMD MI300A. In such systems memory copies between host and device become redundant and programs should implement zero-copy. 
