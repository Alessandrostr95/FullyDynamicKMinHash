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
void experiment4();
void experiment5();
void experiment6();

int main(int argc, char const *argv[])
{
  // experiment1();
  // experiment2();
  // experiment3();
  // experiment4();
  experiment5();
  // experiment6();

  // uint32_t U = 1000000;
  // float p = 0.01;
  // __type *A = create(U, p);
  // cout << count_one(A, U) << " : " << U * p << endl;

  // uint32_t c = 0;
  // for (int i = 0; i < U; i++)
  //   c += get(A, i);
  // cout << c << endl;

  // float p1 = 0.005;
  // float p2 = 0.01;
  // __type *B = perturbate(A, U, p1, p2);

  // uint32_t sizeA = count_one(A, U);
  // uint32_t sizeB = count_one(B, U);

  // cout << sizeB << " " << sizeA - sizeA * p1 + (U - sizeA) * p2 << endl;

  // cout << jaccard_sim(A, B, U) << endl;

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

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
    for (int n = 0; n < n_tests; n++)
      testKLMinhashQuery(l, size, n_query, K[i]);

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 6; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSQuery(K[i], size, n_query, K[i]);

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

  int size = 1 << 14;
  int n_hashes = 1024;
  int l = 32;
  int c = 1024;
  int n_tests = 100;
  float p[15] = {0.001, .005, .01, .05, .1, .15, .2, .25, .3, .35, .40, .45, .5, .55, .6};

  cout << "sketch,k,l,N,n_hash,faults,p,time" << endl;

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 15; i++)
    for (int n = 0; n < n_tests; n++)
      testKLMinhashUpdatesAndQuery(n_hashes, l, size, p[i]);

#pragma omp parallel for collapse(2)
  for (int i = 0; i < 15; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSProactiveUpdatesAndQuery(c, size, n_hashes, p[i]);
  
#pragma omp parallel for collapse(2)
  for (int i = 0; i < 15; i++)
    for (int n = 0; n < n_tests; n++)
      testDSSUpdatesAndQuery(c, size, n_hashes, p[i]);
}

void experiment6()
{
  map<float, pair<float, float>> params{
      {0.1, {0.8183, 0.043}},
      {0.15, {0.73885, 0.039}},
      {0.2, {0.667, 0.035}},
      {0.25, {0.598, 0.032}},
      {0.3, {0.5404, 0.028}},
      {0.35, {0.48375, 0.025}},
      {0.4, {0.4252, 0.023}},
      {0.45, {0.379, 0.02}},
      {0.5, {0.329, 0.018}},
      {0.55, {0.29325, 0.015}},
      {0.6, {0.2518, 0.013}},
      {0.65, {0.21415, 0.011}},
      {0.7, {0.1803, 0.009}},
      {0.75, {0.136, 0.008}},
      {0.8, {0.1088, 0.006}},
      {0.85, {0.0854, 0.004}},
      {0.9, {0.0487, 0.003}},
  };

  int k = 1024;
  int c = 1024;
  int l = 32;
  int U = 100000;
  int n_test = 1000;

  TabulationHash<uint32_t> **hashes = (TabulationHash<uint32_t> **)malloc(k * sizeof(TabulationHash<uint32_t> *));
  for (int i = 0; i < k; i++)
    hashes[i] = new TabulationHash<uint32_t>();

  PairWiseHash<uint32_t> *h1 = new PairWiseHash<uint32_t>();
  PairWiseHash<uint32_t> *h2 = new PairWiseHash<uint32_t>(c);

  cout << "sim, DMH, DSS" << endl;

  for (auto itr = params.begin(); itr != params.end(); itr++)
  {
    float j = itr->first;
    float p1 = itr->second.first;
    float p2 = itr->second.second;

    double err_DMH = 0.0;
    double err_DSS = 0.0;

#pragma omp parallel for reduction(+ : err_DMH, err_DSS)
    for (int n = 0; n < n_test; n++)
    {
      err_DMH += SE_DMH(k, l, U, p1, p2, (Hash<uint32_t> **)hashes);
      err_DSS += SE_DSS(c, c, U, p1, p2, (Hash<uint32_t> **)hashes, (Hash<uint32_t> *)h1, (Hash<uint32_t> *)h2);
    }

    err_DMH = sqrt(err_DMH / (double)n_test);
    err_DSS = sqrt(err_DSS / (double)n_test);
    printf("%f, %f, %f\n", j, err_DMH, err_DSS);
  }
}
