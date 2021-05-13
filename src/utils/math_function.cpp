#include "math_function.hpp"
#include <omp.h>
#include <emmintrin.h>

template<>
void th_cpu_gemm<int>(const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB,
                 const int M, const int N, const int K, const double alpha,
                 const int *A, const int *B, const double beta, int *C)
{
    for (int m=0; m<M; ++m)
    {
        for (int n=0; n<N; ++n)
        {
            for (int k=0; k<K; ++k)
            {
                *(C+m*N+n) += (*(A+m*K+k)) * (*(B+k*N+n));
            }
        }
    }
}

template<>
void th_cpu_gemm<float>(const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB,
                 const int M, const int N, const int K, const double alpha,
                 const float *A, const float *B, const double beta, float *C)
{
#if 1
#pragma omp parallel for
    for (int m=0; m<M; ++m)
    {
        for (int n=0; n<N; ++n)
        {
            for (int k=0; k<K; ++k)
            {
                *(C+m*N+n) += (*(A+m*K+k)) * (*(B+k*N+n));
            }
        }
    }
#else
    __m128 dataA, dataB, dataTmp = _mm_setzero_ps();
    float tmp[4];
    for (int m=0; m<M; ++m)
    {
        for (int n=0; n<N; ++n)
        {
            float* pa = (float*)A + m * K;
            float* pb = (float*)B + m * K;
            float* pc = (float*)C + m * N;
            for (int k=0; k<K; k+=4)
            {
                dataA = _mm_load_ps(pa + k);
                dataB = _mm_load_ps(pb + k);
                dataTmp += _mm_mul_ps(dataA, dataB);
            }
            _mm_store_ps(tmp, dataTmp);
            dataTmp = _mm_setzero_ps();
            *(pc+n) = tmp[0] + tmp[1] + tmp[2] + tmp[3];
        }
    }
#endif
}

template<>
void th_cpu_gemm<double>(const CBLAS_TRANSPOSE TransA, const CBLAS_TRANSPOSE TransB,
                 const int M, const int N, const int K, const double alpha,
                 const double *A, const double *B, const double beta, double *C)
{
    for (int m=0; m<M; ++m)
    {
        for (int n=0; n<N; ++n)
        {
            for (int k=0; k<K; ++k)
            {
                *(C+m*N+n) += (*(A+m*K+k)) * (*(B+k*N+n));
            }
        }
    }
}

inline bool is_a_ge_zero_and_a_lt_b(int a, int b) {
  return static_cast<unsigned>(a) < static_cast<unsigned>(b);
}

template <typename Dtype>
void im2col_cpu(const Dtype* data_im, const int channels,
    const int height, const int width, const int kernel_h, const int kernel_w,
    const int pad_h, const int pad_w,
    const int stride_h, const int stride_w,
    const int dilation_h, const int dilation_w,
    Dtype* data_col)
{
    const int output_h = (height + 2 * pad_h - (dilation_h * (kernel_h - 1) + 1)) / stride_h + 1;
    const int output_w = (width + 2 * pad_w - (dilation_w * (kernel_w - 1) + 1)) / stride_w + 1;
    const int channel_size = height * width;
    for (int channel = channels; channel--; data_im += channel_size)
    {
        for (int kernel_row = 0; kernel_row < kernel_h; kernel_row++)
        {
            for (int kernel_col = 0; kernel_col < kernel_w; kernel_col++)
            {
                int input_row = -pad_h + kernel_row * dilation_h;
                for (int output_rows = output_h; output_rows; output_rows--)
                {
                    if (!is_a_ge_zero_and_a_lt_b(input_row, height))
                    {
                        for (int output_cols = output_w; output_cols; output_cols--)
                        {
                            *(data_col++) = 0;
                        }
                    }
                    else
                    {
                        int input_col = -pad_w + kernel_col * dilation_w;
                        for (int output_col = output_w; output_col; output_col--)
                        {
                            if (is_a_ge_zero_and_a_lt_b(input_col, width))
                            {
                                *(data_col++) = data_im[input_row * width + input_col];
                            }
                            else
                            {
                                *(data_col++) = 0;
                            }
                            input_col += stride_w;
                        }
                    }
                    input_row += stride_h;
                }
            }
        }
    }
}
template void im2col_cpu(const int* data_im, const int channels,
    const int height, const int width, const int kernel_h, const int kernel_w,
    const int pad_h, const int pad_w,
    const int stride_h, const int stride_w,
    const int dilation_h, const int dilation_w,
    int* data_col);
template void im2col_cpu(const float* data_im, const int channels,
    const int height, const int width, const int kernel_h, const int kernel_w,
    const int pad_h, const int pad_w,
    const int stride_h, const int stride_w,
    const int dilation_h, const int dilation_w,
    float* data_col);
template void im2col_cpu(const double* data_im, const int channels,
    const int height, const int width, const int kernel_h, const int kernel_w,
    const int pad_h, const int pad_w,
    const int stride_h, const int stride_w,
    const int dilation_h, const int dilation_w,
    double* data_col);
