#include "src/DSS.cpp"
#include "src/LSH.cpp"
#include "src/TreeBottomKL.h"
#include "src/hash.cpp"
#include "src/BitArray.cpp"
#include "src/test/test.cpp"
#include <algorithm>
#include <cstdint>
#include <omp.h>

using namespace std;

void experiment1();
void experiment2();
void experiment3();
void experiment4(); // WIP
void experiment5(); // WIP

int main(int argc, char const *argv[])
{
  // experiment1();
  // experiment2();
  // experiment3();
  // experiment4();
  // experiment5();

  uint32_t U = 1000000;
  float p = 0.01;
  __type *A = create(U, p);
  cout << count_one(A, U) << " : " << U * p << endl;

  uint32_t c = 0;
  for (int i = 0; i < U; i++)
    c += get(A, i);
  cout << c << endl;

  float p1 = 0.005;
  float p2 = 0.01;
  __type *B = perturbate(A, U, p1, p2);

  uint32_t sizeA = count_one(A, U);
  uint32_t sizeB = count_one(B, U);

  cout << sizeB << " " << sizeA - sizeA * p1 + (U - sizeA) * p2 << endl;

  return 0;
}

/**
 * @todo add documentation
 * k-minhash
 * monotone al variare di l
 */
void experiment1()
{
  uint32_t U = UINT32_MAX;
  int K[4] = {1, 100, 1000, 2000};
  int N = 1 << 16;
  // int N = 1 << 10;
  int n_tests = 8;

#pragma omp parallel for collapse(2)

  for (int n = 0; n < n_tests; n++)
  {
    for (int i = 0; i < 4; i++)
    {
      int k = K[i];

      // l = 1
      singleSetImplicit(k, 1, N);

      for (int l = 5; l <= 100; l += 5)
        singleSetImplicit(k, l, N);

      for (int l = 200; l <= 1000; l += 100)
        singleSetImplicit(k, l, N);
    }
  }
}

/**
 * @todo add documentation
 * k-minhash
 * sliding window al variare di l
 */
void experiment2()
{
  uint32_t U = UINT32_MAX;
  int K[4] = {1, 100, 1000, 5000};
  // int N = 1 << 17;
  int N = 1 << 10;
  int max_size = N / 5;
  int n_tests = 10;

#pragma omp parallel for

  for (int n = 0; n < n_tests; n++)
  {
    for (int i = 0; i < 4; i++)
    {
      int k = K[i];

      // l = 1
      slidingWindowMinHash(k, 1, U, 2 * N, max_size);

      for (int l = 5; l <= 100; l += 5)
      {
        slidingWindowMinHash(k, l, U, 2 * N, max_size);
      }

      for (int l = 200; l <= 1000; l += 100)
      {
        slidingWindowMinHash(k, l, U, 2 * N, max_size);
      }
    }
  }
}

/**
 * @todo add documentation
 */
void experiment3()
{
  srand(time(NULL));

  int N = 1 << 16;
  int K[6] = {64, 128, 256, 512, 1024, 2048};
  int l = 32;
  int n_tests = 10;

  cout << "l-buffered k-minhash" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
  {
    for (int n = 0; n < n_tests; n++)
      singleSetImplicit(K[i], l, N);
  }

  cout << "DSS" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
  {
    for (int n = 0; n < n_tests; n++)
      testDSS(K[i], N);
  }

  cout << "DSS proactive" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
  {
    for (int n = 0; n < n_tests; n++)
      testDSSProactive(K[i], N, K[i]);
  }
}

/**
 * @todo add documentation
 */
void experiment4()
{
  srand(time(NULL));

  int size = 1 << 16;
  int K[6] = {64, 128, 256, 512, 1024, 2048};
  int l = 32;
  int n_query = 1 << 16;
  int n_tests = 14;

  cout << "l-buffered k-minhash" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
    for (int n = 0; n < n_tests; n++)
      testKLMinhashQuery(l, size, n_query, K[i]);

  cout << "DSS" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSQuery(K[i], size, n_query, K[i]);

  cout << "DSS Proactive" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSProactiveQuery(K[i], size, n_query, K[i]);
}

/**
 * @todo add documentation
 */
void experiment5()
{
  srand(time(NULL));

  int size = 1 << 16;
  int n_hashes = 1024;
  int l = 32;
  int c = 1024;
  int n_tests = 100;
  float p[9] = {0, .005, .01, .05, .1, .15, .2, .25, .3};

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 9; i++)
    for (int n = 0; n < n_tests; n++)
      testKLMinhashUpdatesAndQuery(n_hashes, l, size, p[i]);

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 9; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSProactiveUpdatesAndQuery(c, size, n_hashes, p[i]);

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 9; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSUpdatesAndQuery(c, size, n_hashes, p[i]);
}
