#include "ParallelInstrumental.h"


ParallelInstrumental::ParallelInstrumental() {
	this->prepareData();
}

bool ParallelInstrumental::isPrime(int num) const {
	bool ok = true;
	for (int i = 2; i <= num / 2; ++i) {
		if (num % i == 0) {
			ok = false;
			break;
		}
	}

	return ok;
}

vec ParallelInstrumental::findDivisors(int num) const {
	vec res;

	for (int i = 2; i <= sqrt(num); i++) {
		if (num % i == 0) {
			if (num / i != i) {
				res.push_back(num / i);
			}

			res.push_back(i);
		}
	}

	return res;
}


void ParallelInstrumental::prepareData() {
	do {
		std::cout << "Enter the dimension (N) = ";
		std::cin >> N;

		std::cout << "Enter the number of compute nodes (THREAD_NUM) = ";
		std::cin >> THREAD_NUM;

	} while (!checkData());

	BLOCK_SIZE   = N / THREAD_NUM;
	CLASSIC_SIZE = (THREAD_NUM - 1) * 2;

}

bool ParallelInstrumental::checkData() const {
	if (N < 7) {
		std::cout << "Dimension (N = " << N << ") is too small\n"
				  << "Parallel computing is not effective for it\n"
				  << "Enter a larger dimension (N)\n\n";

		return false;
	}

	if ((N / THREAD_NUM) < 3) {
		std::cout << "The algorithm is not working for the proportions of the dimension (N) "
			      << "with the number of computing nodes (THREAD_NUM)\n"
				  << "Enter a larger dimension (N), or reduce the number of computing nodes (THREAD_NUM)\n\n";

		return false;
	}

	vec div = findDivisors(N);
	if (isPrime(N) || std::find(div.begin(), div.end(), THREAD_NUM) == div.end()) {
		std::cout << "It is impossible to split the dimension (N = "<< N << ") into the same blocks (BLOCK_SIZE)\n"
				  << "Enter the correct values of dimension (N) and number of computing nodes (THREADNUM)\n\n";

		return false;
	}

	return true;
}

std::tuple<size_t, int, int, int> ParallelInstrumental::getAllFields() const {
	return std::make_tuple(N, THREAD_NUM, BLOCK_SIZE, CLASSIC_SIZE);
}

vec ParallelInstrumental::createVecN() {
	vec a(N);
	std::iota(a.begin(), a.end(), 0);

	return a;
}

vec ParallelInstrumental::createVecRand() {
	std::mt19937 gen;
	gen.seed(static_cast<unsigned int>(time(0)));
	vec a(N);

	#pragma omp parallel for
	for (int i = 0; i < N; i++)
		a[i] = gen() % 100;

	return a;
}

matr ParallelInstrumental::createThirdDiagMatrI() {
	matr a(N, vec(N));

	#pragma omp parallel for if (N > 500)
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			a[i][i] = 3.;
			a[i][i - 1] = 1.;
			a[i - 1][i] = 2.;
		}
	}

	a[0][0] = 1.; a[N - 1][N - 1] = 1.;
	a[0][1] = -0.5; a[N - 1][N - 2] = -0.5;

	return a;
}

matr ParallelInstrumental::createThirdDiagMatrRand() {
	std::mt19937 gen;
	gen.seed(static_cast<unsigned int>(time(0)));
	matr a(N, vec(N));

	a[0][0] = gen() % 100;
	#pragma omp parallel for if (N > 500)
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			a[i][i] = gen() % 100;
			a[i][i - 1] = gen() % 100;
			a[i - 1][i] = gen() % 100;
		}
	}

	return a;
}

matr ParallelInstrumental::createMatr(double a, double b, double c) {
	matr res(N, vec(N));

	#pragma omp parallel for if (N > 500)
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			res[i][i] = c;
			res[i][i - 1] = b;
			res[i - 1][i] = a;
		}
	}

	res[0][0] = 1.; res[N - 1][N - 1] = 1.;
	res[0][1] = 0.; res[N - 1][N - 2] = 0.;

	return res;
}

vec ParallelInstrumental::calcMatrVecMult(const matr& A, const vec& b) {
	int n = A.size();
	vec res(n);

	#pragma omp parallel shared(res, n)
	{
		#pragma omp for
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				res[i] += A[i][j] * b[j];
	}

	return res;
}