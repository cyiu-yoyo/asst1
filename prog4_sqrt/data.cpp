#include <algorithm>

// Generate random data
void initRandom(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // random input values
        values[i] = .001f + 2.998f * static_cast<float>(rand()) / RAND_MAX;
    }
}

// Generate data that gives high relative speedup
void initGood(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // get the same number for all to guarantee the
        // even workload, and large workload to mitigate
        // the overhead of SIMD
        values[i] = 2.9999998f;
    }
}

// Generate data that gives low relative speedup
void initBad(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // to get a bad speedup, we can leverage the vectorization
        // batch process, and make others wait for the single one
        // in the same vector
        if (i % 8 == 0){
            values[i] = 2.9999998f;
        }else{
            values[i] = 1.0f;
        }
    }
}

