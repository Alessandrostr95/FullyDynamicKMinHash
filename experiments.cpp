#include "src/DSS.cpp"
#include "src/LSH.cpp"
#include "src/TreeBottomKL.h"
#include "src/hash.cpp"
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
  experiment5();
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
  int N = 1 << 17;
  // int N = 1 << 10;
  int n_tests = 8;

#pragma omp parallel for collapse(2)

  for (int n = 0; n < n_tests; n++)
  {
    for (int i = 0; i < 4; i++)
    {
      int k = K[i];

      // l = 1
      singleSetImplicit(k, 1, U, N);

      for (int l = 5; l <= 100; l += 5)
      {
        singleSetImplicit(k, l, U, N);
      }
      for (int l = 200; l <= 1000; l += 100)
      {
        singleSetImplicit(k, l, U, N);
      }
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

  uint32_t U = UINT32_MAX;
  // int N = 1 << 10;
  int N = 1 << 16;
  int k = 1024;
  int l = 32;
  int c = k;
  int n_tests = 32;

  cout << "l-buffered k-minhash" << endl;

#pragma omp parallel for
  for (int n = 0; n < n_tests; n++)
  {
    singleSetImplicit(k, l, U, N);
    // slidingWindowMinHash(k, l, U, N, N);
  }

  cout << "DSS" << endl;

#pragma omp parallel for
  for (int n = 0; n < n_tests; n++)
  {
    testDSS(c, N);
  }

  cout << "DSS proactive" << endl;

#pragma omp parallel for
  for (int n = 0; n < n_tests; n++)
  {
    testDSSProactive(c, N, k);
  }
}

/**
 * @todo add documentation
 */
void experiment4()
{
  srand(time(NULL));

  uint32_t U = UINT32_MAX;
  // int N = 1 << 10;
  int size = 1 << 16;
  int n_hashes = 1000;
  int l = 32;
  int n_query = 1 << 16;
  int c = 1024;
  int n_tests = 14;

  cout << "l-buffered k-minhash" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
  {
    testKLMinhashQuery(l, size, n_query, n_hashes);
  }

  cout << "DSS" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
  {
    testDSSQuery(c, size, n_query, n_hashes);
  }

  cout << "DSS Proactive" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
  {
    testDSSProactiveQuery(c, size, n_query, n_hashes);
  }
}

/**
 * @todo add documentation
 */
void experiment5()
{
  srand(time(NULL));

  uint32_t U = UINT32_MAX;
  // int N = 1 << 10;
  int size = 1 << 16;
  int n_hashes = 1000;
  int l = 32;
  int c = 1024;
  int n_tests = 14;
  float p = 0.1;

  cout << "l-buffered k-minhash" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
    testKLMinhashUpdatesAndQuery(n_hashes, l, U, size, p);

  cout << "DSS Proactive" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
    testDSSProactiveUpdatesAndQuery(c, size, n_hashes, p);

  cout << "DSS" << endl;

#pragma omp parallel for
  for (int i = 0; i < n_tests; i++)
    testDSSUpdatesAndQuery(c, size, n_hashes, p);
}
