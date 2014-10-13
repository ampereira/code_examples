#include <cstdlib>
#include <iostream>
#include <spa.h>
#include <immintrin.h>

#define SIZE 128
#ifdef AVX
	#define VEC_SIZE SIZE/8
#elif SSE
	#define VEC_SIZE SIZE/4
#endif

/*
 *	Example of matrix-matrix arithmetic (addition and multiplication)
 *
 *	Students are supposed to test/implement the following optimisations
 *	for the multiplication:
 *		-> use a local variable to put the result on registers before it's
 *		   not used anymore
 *		-> 
 *		-> Tiling
 *		-> use gcc auto vectorisation and analyse the report
 *		-> use gcc -O1,2,3
 */

using namespace std;

float m1[SIZE][SIZE], m2[SIZE][SIZE], result[SIZE][SIZE];

void fillMatrices (void) {

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < SIZE; ++j) {
			m1[i][j] = ((float) rand()) / ((float) RAND_MAX);
			m2[i][j] = ((float) rand()) / ((float) RAND_MAX);
		}
	}
}

// Unoptimised version
void matrixMultNaive (void) {

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < SIZE; ++j) {
			result[i][j] = 0;
			for (unsigned k = 0; k < SIZE; ++k) {
				result[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
}

// First optimisation
// uses a temporary value to avoid unecessary writes to memory
// (the compiler does not optimise as result may be an alias of m1 or m2)
void matrixMultOpt1 (void) {

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < SIZE; ++j) {
			float x = 0;
			for (unsigned k = 0; k < SIZE; ++k) {
				x += m1[i][k] * m2[k][j];
			}
			result[i][j] = x;
		}
	}
}

void matrixAddNaive (void) {
	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < SIZE; ++j) {
			result[i][j] = m1[i][j] + m2[i][j];
		}
	}
}

// Using avx intrinsics
#ifdef AVX
void matrixAddVec (void) {
	__m256 ymm1, ymm2;

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < VEC_SIZE; ++j) {
			ymm1 = _mm256_load_ps(&m1[i][j * 8]);
			ymm2 = _mm256_load_ps(&m2[i][j * 8]);

			ymm1 = _mm256_add_ps(ymm1, ymm2);
			_mm256_store_ps(&result[i][j * 8], ymm1);
		}
	}
}
#endif

// Using sse intrinsics
#ifdef SSE
void matrixAddVec (void) {
	__m128 ymm1, ymm2;

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < VEC_SIZE; ++j) {
			ymm1 = _mm_load_ps(&m1[i][j * 4]);
			ymm2 = _mm_load_ps(&m2[i][j * 4]);

			ymm1 = _mm_add_ps(ymm1, ymm2);
			_mm_store_ps(&result[i][j * 4], ymm1);
		}
	}
}
#endif

int main (int argc, char *argv[]) {
	Measure meas1, meas2;

	if (argc < 5) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}

	float th = atof(argv[1]);
	unsigned k = atoi(argv[2]);
	unsigned min = atoi(argv[3]);
	unsigned max = atoi(argv[4]);

	fillMatrices();

	meas1.kbest(matrixAddNaive, th, k, min, max);
	meas1.report(Report::Verbose);
	meas2.kbest(matrixAddVec, th, k, min, max);
	meas2.report(Report::Verbose);

	return 1;
}