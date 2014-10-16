#include <cstdlib>
#include <iostream>
#include <spa.h>
#include <immintrin.h>

#define SIZE 1024	// Only powers of 2 to simplify the code

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
 *		-> use icc auto vectorisation and analyse the report
 *		-> use icc -O1,2,3
 */

using namespace std;

float m1[SIZE][SIZE], m2[SIZE][SIZE], result[SIZE][SIZE];

void fillMatrices (void) {

#pragma ivdep
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

// What is the difference from the Opt1?
// Why is this slower/faster?
void matrixMultOpt1v2 (void) {
	float x;

	for (unsigned i = 0; i < SIZE; ++i) {
		for (unsigned j = 0; j < SIZE; ++j) {
			x = 0;
			for (unsigned k = 0; k < SIZE; ++k) {
				x += m1[i][k] * m2[k][j];
			}
			result[i][j] = x;
		}
	}
}


// Second optimisation - they stack!
// uses blocking to improve cache usage
// find the best block size!
// optional: change code to allow for any block size
void matrixMultOpt2 (void) {
	float x;
	unsigned s = 16;	// block size s * s

	for(unsigned jj = 0; jj < SIZE; jj += s) {
		for(unsigned kk = 0; kk < SIZE; kk += s) {
			for(unsigned i = 0; i < SIZE; ++i) {
//				for(unsigned j = jj; j < ((jj + s) > SIZE ? SIZE : (jj + s)); ++j) {
				for(unsigned j = jj; j < jj + s; ++j) {
					x = 0;
//					for(unsigned k = kk; k < ((kk + s) > SIZE ? SIZE : (kk + s)); ++k) {
					for(unsigned k = kk; k < kk + s; ++k) {
						x += m1[i][k] * m2[k][j];
					}
					result[i][j] += x;
				}
			}
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

// Uses only one loop
// Easier to vectorise... But why is it slower?
void matrixAddOpt1 (void) {
	unsigned size = SIZE * SIZE;

	for (unsigned i = 0; i < size; ++i)
			result[(int)(i / SIZE - 0.5)][i % SIZE] = m1[(int)(i / SIZE - 0.5)][i % SIZE] + m2[(int)(i / SIZE - 0.5)][i % SIZE];
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

	meas1.kbest(matrixMultNaive, th, k, min, max);
	meas1.report(Report::Verbose);
	meas2.kbest(matrixMultOpt2, th, k, min, max);
	meas2.report(Report::Verbose);

	return 1;
}