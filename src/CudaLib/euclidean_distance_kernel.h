/**
 * @file   CudaLib/euclidean_distance_kernel.h
 * @date   Nov 23, 2018
 * @author Bernd Doser, HITS gGmbH
 */

#pragma once

#include <cstdint>
#include "sm_61_intrinsics.h"

namespace pink {

/// CUDA device kernel for reducing a data array with the length of 64 by
/// static loop unrolling for the thread within one warp
template <typename DataType>
__device__
void warp_reduce_64(volatile DataType *data, int tid)
{
    data[tid] += data[tid + 32];
    data[tid] += data[tid + 16];
    data[tid] += data[tid +  8];
    data[tid] += data[tid +  4];
    data[tid] += data[tid +  2];
    data[tid] += data[tid +  1];
}

/// CUDA device kernel to computes the euclidean distance of two arrays
/// using a reduced type to calculate the euclidean distance
template <uint16_t block_size, typename DataType, typename EuclideanType>
__global__
void euclidean_distance_kernel(EuclideanType const *som, EuclideanType const *rotated_images,
    DataType *first_step, uint32_t neuron_size);

template <>
__global__
void euclidean_distance_kernel<256>(float const *som, float const *rotated_images, float *first_step, uint32_t neuron_size)
{
    int tid = threadIdx.x;
    float diff;
    float sum = 0.0;
    float const *psom = som + blockIdx.y * neuron_size;
    float const *prot = rotated_images + blockIdx.x * neuron_size;

    __shared__ float first_step_local[256];

    for (uint32_t i = tid; i < neuron_size; i += 256)
    {
        diff = psom[i] - prot[i];
        sum += diff * diff;
    }

    first_step_local[tid] = sum;
    __syncthreads();

    // Parallel reduction
    if (tid < 128) { first_step_local[tid] += first_step_local[tid + 128]; } __syncthreads();
    if (tid <  64) { first_step_local[tid] += first_step_local[tid +  64]; } __syncthreads();

    // Static loop unrolling for the thread within one warp.
    if (tid < 32) warp_reduce_64(first_step_local, tid);

    // Copy accumulated local value to global array first_step
    if (tid == 0) first_step[blockIdx.x + blockIdx.y * gridDim.x] = first_step_local[0];
}


template <>
__global__
void euclidean_distance_kernel<256>(uint8_t const *som, uint8_t const *rotated_images, float *first_step, uint32_t neuron_size)
{
    int tid = threadIdx.x;
    float sum = 0.0;
    uint8_t const *psom = som + blockIdx.y * neuron_size;
    uint8_t const *prot = rotated_images + blockIdx.x * neuron_size;

    __shared__ float first_step_local[256];

#if defined(__CUDA_ARCH__) && (__CUDA_ARCH__ >= 610)
    uint32_t null = 0;
    for (uint32_t i = tid; i < neuron_size; i += 256)
    {
        uint32_t diff = std::abs(psom[i] - prot[i]);
        uint32_t j;
        for (j = 0, i += 256; j < 3 && i < neuron_size; ++j, i += 256)
        {
            (diff << 8) | std::abs(psom[i] - prot[i]);
        }

        sum += __dp4a(diff, diff, null);
    }
#else
    float diff;
    for (uint32_t i = tid; i < neuron_size; i += 256)
    {
        diff = psom[i] - prot[i];
        sum += diff * diff;
    }
#endif

    first_step_local[tid] = sum;
    __syncthreads();

    // Parallel reduction
    if (tid < 128) { first_step_local[tid] += first_step_local[tid + 128]; } __syncthreads();
    if (tid <  64) { first_step_local[tid] += first_step_local[tid +  64]; } __syncthreads();

    // Static loop unrolling for the thread within one warp.
    if (tid < 32) warp_reduce_64(first_step_local, tid);

    // Copy accumulated local value to global array first_step
    if (tid == 0) first_step[blockIdx.x + blockIdx.y * gridDim.x] = first_step_local[0];
}


} // namespace pink
