#include <smmintrin.h> // For _mm_stream_load_si128
#include <emmintrin.h> // For _mm_mul_ps
#include <assert.h>
#include <stdint.h>

extern void saxpySerial(int N,
			float scale,
			float X[],
			float Y[],
			float result[]);


void saxpyStreaming(int N,
                    float scale,
                    float X[],
                    float Y[],
                    float result[])
{
    __m128 vecA = _mm_set1_ps(scale);

    // assume that N is dividible by 4
    for(int i = 0; i < N; i += 4){
        // loading
        __m128 xVal=(__m128)_mm_stream_load_si128((__m128i*)&X[i]);
        __m128 yVal=(__m128)_mm_stream_load_si128((__m128i*)&Y[i]);
        // calculation
        __m128 tmp = _mm_mul_ps(vecA,xVal);
        tmp = _mm_add_ps(tmp, yVal);
       _mm_stream_ps(&result[i],tmp);
    }
}

