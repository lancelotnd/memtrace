#include <hip/hip_runtime.h>
#include <iostream>

#define N 1048576 // Size of the array

// Check for errors
#define HIP_CHECK(call) {                                      \
    hipError_t err = call;                                      \
    if (err != hipSuccess) {                                    \
        std::cerr << "HIP Error: " << hipGetErrorString(err)     \
                  << " at line " << __LINE__ << std::endl;      \
        exit(-1);                                               \
    }                                                           \
}

int main() {
    // Allocate and initialize host memory
    int *h_data = new(std::nothrow) int[N];
    if (!h_data) {
        std::cerr << "Host memory allocation failed!" << std::endl;
        return -1;
    }

    for (int i = 0; i < N; ++i) {
        h_data[i] = i;
    }

    // Allocate device memory
    int *d_data;
    HIP_CHECK(hipMalloc((void**)&d_data, N * sizeof(int)));

    // Create a stream for asynchronous operations
    hipStream_t stream;
    HIP_CHECK(hipStreamCreate(&stream));

    // Copy data from host to device asynchronously
    HIP_CHECK(hipMemcpyAsync(d_data, h_data, N * sizeof(int), hipMemcpyHostToDevice, stream));

    // Allocate memory to store data back on the host
    int *h_data_check = new(std::nothrow) int[N];
    if (!h_data_check) {
        std::cerr << "Host memory allocation failed for h_data_check!" << std::endl;
        return -1;
    }

    // Copy data from device to host asynchronously
    HIP_CHECK(hipMemcpyAsync(h_data_check, d_data, N * sizeof(int), hipMemcpyDeviceToHost, stream));

    // Wait for the stream to finish before proceeding
    HIP_CHECK(hipStreamSynchronize(stream));

    // Verify the data
    for (int i = 0; i < N; ++i) {
        if (h_data_check[i] != h_data[i]) {
            std::cerr << "Data mismatch at index " << i << ": "
                    << h_data_check[i] << " != " << h_data[i] << std::endl;
            return -1;
        }
    }

    std::cout << "Data verification successful. All values match!" << std::endl;

    // Clean up
    HIP_CHECK(hipFree(d_data));
    HIP_CHECK(hipStreamDestroy(stream));

    delete[] h_data;
    delete[] h_data_check;

    return 0;
}
