#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "CMU418intrin.h"
#include "logger.h"
using namespace std;


void absSerial(float* values, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	if (x < 0) {
	    output[i] = -x;
	} else {
	    output[i] = x;
	}
    }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float* values, float* output, int N) {
    __cmu418_vec_float x;
    __cmu418_vec_float result;
    __cmu418_vec_float zero = _cmu418_vset_float(0.f);
    __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;

    //  Note: Take a careful look at this loop indexing.  This example
    //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
    //  Why is that the case?
	// when N % VECTOR_WIDTH != 0, but we still + VECTOR_WIDTH, we may 
	// fail to handle the last part of the arraies.

	// fixed version:
    for (int i=0; i<=N; i+=VECTOR_WIDTH) {
	
	if ((N - i) < VECTOR_WIDTH){
		// N % VECTOR_WIDTH != 0
		// handle last part of the array by using special mask
		maskAll = _cmu418_init_ones(N-i);
	} else {
		// All ones
		maskAll = _cmu418_init_ones();
	}
	// All ones
	// maskAll = _cmu418_init_ones();

	// All zeros
	maskIsNegative = _cmu418_init_ones(0);

	// Load vector of values from contiguous memory addresses
	_cmu418_vload_float(x, values+i, maskAll);               // x = values[i];

	// Set mask according to predicate
	_cmu418_vlt_float(maskIsNegative, x, zero, maskAll);     // if (x < 0) {

	// Execute instruction using mask ("if" clause)
	_cmu418_vsub_float(result, zero, x, maskIsNegative);      //   output[i] = -x;

	// Inverse maskIsNegative to generate "else" mask
	maskIsNotNegative = _cmu418_mask_not(maskIsNegative);     // } else {

	// Execute instruction ("else" clause)
	_cmu418_vload_float(result, values+i, maskIsNotNegative); //   output[i] = x; }

	// Write results back to memory
	_cmu418_vstore_float(output+i, result, maskAll);
    }

}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float* values, int* exponents, float* output, int N) {
    for (int i=0; i<N; i++) {
		float x = values[i];
		float result = 1.f;
		int y = exponents[i];
		float xpower = x;
		while (y > 0) {
			if (y & 0x1) {
				result *= xpower;
			}
			xpower = xpower * xpower;
			y >>= 1;
		}
		if (result > 4.18f) {
			result = 4.18f;
		}
		output[i] = result;
    }
}

void clampedExpVector(float* values, int* exponents, float* output, int N) {
	__cmu418_vec_float x, result, xpower; // for float values and result for each value, xpower
	__cmu418_vec_float clampValue; // for clamp value 4.18f
	__cmu418_vec_int allZeros, allOnes, yLastBit;
	__cmu418_vec_int y; // for int exponents
	__cmu418_mask maskAll, maskYStatus, maskYLastBit, maskClamp;

	clampValue = _cmu418_vset_float(4.18f);
	allZeros = _cmu418_vset_int(0);
	allOnes = _cmu418_vset_int(1);
	for (int i = 0; i <= N; i+= VECTOR_WIDTH){
		if ((N - i) < VECTOR_WIDTH){
			// N % VECTOR_WIDTH != 0
			// handle last part of the array by using special mask
			maskAll = _cmu418_init_ones(N-i);
		} else {
			// All ones
			maskAll = _cmu418_init_ones();
		}	

		// load values
		_cmu418_vload_float(x, values + i, maskAll);
        _cmu418_vload_int(y, exponents + i, maskAll);
		result = _cmu418_vset_float(1.0f);
		xpower = x;

		maskYStatus = _cmu418_init_ones(0);
		_cmu418_vgt_int(maskYStatus, y, allZeros, maskAll);
		while(_cmu418_cntbits(maskYStatus) != 0){

			//check least significant bit of y: if(y & 0x1)
			yLastBit = _cmu418_vset_int(0);
			_cmu418_vbitand_int(yLastBit, y, allOnes, maskYStatus);
			maskYLastBit = _cmu418_init_ones(0);
			_cmu418_vgt_int(maskYLastBit, yLastBit, allZeros, maskYStatus);

			//conditional result *= xpower;
			_cmu418_vmult_float(result, result, xpower, maskYLastBit);
			//conditional xpower = xpower * xpower;
			_cmu418_vmult_float(xpower, xpower, xpower, maskYStatus);	

			//rightshift
			_cmu418_vshiftright_int(y, y, allOnes, maskYStatus);
			//calculate maskYStatus
			_cmu418_vgt_int(maskYStatus, y, allZeros, maskYStatus);	
		}	

		// Clamp the result
		// if (result > 4.18f)
		maskClamp = _cmu418_init_ones(0);	
		_cmu418_vgt_float(maskClamp, result, clampValue, maskAll);
		_cmu418_vmove_float(result, clampValue, maskClamp);

		//write result back to memory
		_cmu418_vstore_float(output+i, result, maskAll);
		
	}
}


float arraySumSerial(float* values, int N) {
    float sum = 0;
    for (int i=0; i<N; i++) {
	sum += values[i];
    }

    return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float* values, int N) {
    // Implement your vectorized version here
    //  ...
	return 0.f;
}
