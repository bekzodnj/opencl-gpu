#include <iostream>
#include <vector>
#include "CL/cl.h"

int main() {

    // getting PlatformID
    cl_uint num_entries = 1;
    cl_platform_id platforms = 0;
    cl_uint num_platforms = 1;
    clGetPlatformIDs(num_entries, &platforms, &num_platforms);

    // getting DeviceID
    cl_device_id device = 0;
    clGetDeviceIDs(platforms, CL_DEVICE_TYPE_GPU,  1, &device, nullptr);

    // getting a Context
    cl_context context;
    cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platforms, 0};
    context = clCreateContext(properties, 1, &device, nullptr, nullptr, nullptr);

    // getting a Queue to pass commands for the device
    cl_command_queue queue;
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, nullptr);


    // usually kernel funtions will be in the file
    // file reader utility function can be used, but here we can use simple string
    // kernel functions will not have a return type
    std::string source = "__kernel void square(__global float* input, __global float* output){"
                         "int id = get_global_id(0);"
                         "output[id] = input[id] * input[id];"
                         "}";


    // now program can be created by using kernel source
    const char* cstring = source.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &cstring, nullptr, nullptr);
    int error = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

    if( error != CL_SUCCESS){
        std::cerr << "Something went wrong" << std::endl;
    }

    // now kernel object can be created
    cl_kernel kernel = clCreateKernel(program, "square", &error);

    // to show example of squre use some list of numbers
    std::vector<float> hInput;
    size_t numElements = 128;
    for(int i = 0; i<numElements; i++){
        hInput.push_back(i);
    }

    // we need memory obj. in the OpenCL devices global memory
    cl_mem inputBuffer;
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * numElements, nullptr, &error);

    // copy the data from host to the device. Args: True - blocking flag; 0 - offset
    clEnqueueWriteBuffer(queue, inputBuffer, CL_TRUE, 0, sizeof(float) * numElements, hInput.data(), 0, nullptr, nullptr);

    cl_mem outputBuffer;
    outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(float) * numElements, nullptr, &error);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);

    clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &numElements, nullptr, 0, nullptr, nullptr);

    // sync host and opencl device
    clFinish(queue);

    clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, sizeof(float) * numElements, hInput.data(), 0, nullptr, nullptr);


    std::cout << "Hello, World!" << std::endl;

    return 0;

}
