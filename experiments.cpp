#include "src/DSS.cpp"
#include "src/LSH.cpp"
#include "src/TreeBottomKL.h"
#include "src/hash.cpp"
#include "src/BitArray.cpp"
#include "src/test/test.cpp"
#include <algorithm>
#include <cstdint>
#include <omp.h>
#include "src/Utils.cpp"

#include "src/TreeKLMinhash.h"
#include "src/DSS.cpp"

using namespace std;

void experiment1();
void experiment2();
void experiment3();
void experiment4();
void experiment5();
void experiment6();
void experiment7(string, double, int, int);

int main(int argc, char const *argv[])
{
  // experiment1();
  // experiment2();
  // experiment3();
  // experiment4();
  // experiment5();
  // experiment6();
  experiment7("dataset_Slashdot0811.txt", 0.1, 70, 3);
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
  // int l = 32;
  int U = 1 << 17;
  int l = 17;
  int n_test = 100;

  TabulationHash<uint32_t> **hashes = (TabulationHash<uint32_t> **)malloc((k * l) * sizeof(TabulationHash<uint32_t> *));
  for (int i = 0; i < (k * l); i++)
    hashes[i] = new TabulationHash<uint32_t>();

  // PairWiseHash<uint32_t> *h1 = new PairWiseHash<uint32_t>();
  PairWiseHash<uint32_t> *h1 = new PairWiseHash<uint32_t>();
  PairWiseHash<uint32_t> *h2 = new PairWiseHash<uint32_t>(c);

  cout << "sim,DMH,DSS,min_hash" << endl;

  for (auto itr = params.begin(); itr != params.end(); itr++)
  {
    float j = itr->first;
    float p1 = itr->second.first;
    float p2 = itr->second.second;

    double err_DMH = 0.0;
    double err_DSS = 0.0;
    double err_min_hash = 0.0;

#pragma omp parallel for // reduction(+ : err_DMH, err_DSS)
    for (int n = 0; n < n_test; n++)
    {
      err_DMH = SE_DMH(k, l, U, p1, p2, (Hash<uint32_t> **)hashes);
      err_DSS = SE_DSS(c, c, U, p1, p2, (Hash<uint32_t> **)hashes, (Hash<uint32_t> *)h1, (Hash<uint32_t> *)h2);
      err_min_hash = SE_DMH(k * l, 1, U, p1, p2, (Hash<uint32_t> **)hashes);

      printf("%f, %f, %f, %f\n", j, err_DMH, err_DSS, err_min_hash);
    }

    // err_DMH = sqrt(err_DMH / (double)n_test);
    // err_DSS = sqrt(err_DSS / (double)n_test);
    // err_min_hash = sqrt(err_min_hash / (double)n_test);
    // printf("%f, %f, %f, %f\n", j, err_DMH, err_DSS, err_min_hash);
  }
}

/**
 * Test ACP
 */
void experiment7(std::string fileName, double J, int b, int r)
{
  cout << "Loading dataset..." << endl;

  // load data set
  std::unordered_map<int, set<int> *> *sets = loadSetsFromFile(fileName);
  vector<int> setIds;

  for (auto itr = sets->begin(); itr != sets->end(); itr++)
  {
    set<int> *S = itr->second;
    int id = itr->first;
    setIds.push_back(id);
  }

  int n = sets->size();

  // compute true positive
  std::set<pair<int, int>> positive;
  for (int i = 0; i < setIds.size() - 1; i++)
  {
    for (int j = i + 1; j < setIds.size(); j++)
    {
      auto A = sets->find(setIds[i])->second;
      auto B = sets->find(setIds[j])->second;
      if (jaccard(A, B) >= J)
      {
        if (setIds[i] > setIds[j])
          positive.insert({setIds[j], setIds[i]});
        else
          positive.insert({setIds[i], setIds[j]});
      }
    }
  }

  // printf("There is %d pairs with JS >= %f\n", truePositive.size(), J);

  cout << "Setup experiment..." << endl;

  // define hash functions
  int k = b * r;
  int l = 1;
  int c = k;
  TabulationHash<uint32_t> **hashes = (TabulationHash<uint32_t> **)malloc((k * l) * sizeof(TabulationHash<uint32_t> *));
  for (int i = 0; i < (k * l); i++)
    hashes[i] = new TabulationHash<uint32_t>();

  // PairWiseHash<uint32_t> *h1 = new PairWiseHash<uint32_t>();
  PairWiseHash<uint32_t> *h1 = new PairWiseHash<uint32_t>();
  PairWiseHash<uint32_t> *h2 = new PairWiseHash<uint32_t>(c);

  // create sketche
  uint32_t **signaturesBMH = (uint32_t **)malloc(sizeof(uint32_t *) * n);
  uint32_t **signaturesDSS = (uint32_t **)malloc(sizeof(uint32_t *) * n);
  int i = 0;

  for (auto itr = sets->begin(); itr != sets->end(); itr++)
  {
    set<int> *set = itr->second;
    int id = itr->first;

    TreeKLMinhash *S1 = new TreeKLMinhash(k, l, UINT32_MAX, (Hash<uint32_t> **)hashes, false);
    DSS *S2 = new DSS(c, h1, h2, (Hash<uint32_t> **)hashes, k, false);

    for (auto el = set->begin(); el != set->end(); el++)
    {
      S1->insert(*el);
      S2->insert(*el);
    }

    signaturesBMH[i] = S1->getSignature();
    signaturesDSS[i] = S2->getSignature();
    i++;
  }

  cout << "Starting LSH..." << endl;

  // compute LSH
  cout << "NOI" << endl;
  unordered_set<pair<int, int>, hash_pair> *candidatePairsBMH = computeLSH(signaturesBMH, n, r, b);
  cout << "SORELLA" << endl;
  unordered_set<pair<int, int>, hash_pair> *candidatePairsDSS = computeLSH(signaturesDSS, n, r, b);

  cout << "Effective pairs:" << positive.size() << endl;
  cout << "BMH pairs:" << candidatePairsBMH->size() << endl;
  cout << "DSS pairs:" << candidatePairsDSS->size() << endl
       << endl;

  // compute statistics
  int TP_BMH = 0;
  for (auto itr = candidatePairsBMH->begin(); itr != candidatePairsBMH->end(); itr++)
  {
    TP_BMH += (positive.find({setIds[itr->first], setIds[itr->second]}) != positive.end()) || (positive.find({setIds[itr->second], setIds[itr->first]}) != positive.end());
  }
  int FP_BMH = candidatePairsBMH->size() - TP_BMH;
  int FN_BMH = positive.size() - TP_BMH;

  int TP_DSS = 0;
  for (auto itr = candidatePairsDSS->begin(); itr != candidatePairsDSS->end(); itr++)
  {
    if ((positive.find({setIds[itr->first], setIds[itr->second]}) != positive.end()) || (positive.find({setIds[itr->second], setIds[itr->first]}) != positive.end()))
    {
      TP_DSS++;
    }
  }
  int FP_DSS = candidatePairsDSS->size() - TP_DSS;
  int FN_DSS = positive.size() - TP_DSS;

  // compute precision and recall
  double precision_BMH = ((double)TP_BMH) / ((double)TP_BMH + FP_BMH);
  double recall_BMH = ((double)TP_BMH) / ((double)TP_BMH + FN_BMH);
  double F1_BMH = 2 * (precision_BMH * recall_BMH) / (precision_BMH + recall_BMH);

  double precision_DSS = ((double)TP_DSS) / ((double)TP_DSS + FP_DSS);
  double recall_DSS = ((double)TP_DSS) / ((double)TP_DSS + FN_DSS);
  double F1_DSS = 2 * (precision_DSS * recall_DSS) / (precision_DSS + recall_DSS);

  cout << "Buffered MinHash" << endl;
  printf("Precision: %f\nRecall: %f\nF1: %f\n\n", precision_BMH, recall_BMH, F1_BMH);

  cout << "Sorella" << endl;
  printf("Precision: %f\nRecall: %f\nF1: %f\n\n", precision_DSS, recall_DSS, F1_DSS);
}