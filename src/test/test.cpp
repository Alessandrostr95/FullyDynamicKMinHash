#include "../TreeKLMinhash.h"
#include "../DSS.cpp"
#include "../DSSProactive.cpp"
#include "../TreeBottomKL.h"
#include "../LSH.cpp"
#include "../BitArray.cpp"
#include <algorithm>
#include <chrono>
using namespace std::chrono;
using namespace std;

void permute(int *a, int n)
{
    random_shuffle(a, a + n);
}

uint32_t *generate_random_sample(uint32_t N)
{
    unordered_set<uint32_t> values = unordered_set<uint32_t>(N);
    std::random_device rd;                               // Obtain a random seed from the hardware
    std::mt19937 rng(rd());                              // Create a generator instance with the seed
    std::uniform_int_distribution<uint32_t> uint_dist_n; //  range [0, n]

    uint32_t *sample = new uint32_t[N];
    int i = 0;

    while (i < N)
    {
        uint32_t x = uint_dist_n(rng);
        if (values.insert(x).second)
            sample[i++] = x;
    }

    return sample;
}

/**
 * @todo add documentation
 */
void singleSetImplicit(int k, int l, int N)
{
    int n_fault = 0;

    TreeKLMinhash *S = new TreeKLMinhash(k, l, UINT32_MAX, false);
    uint32_t *sample = generate_random_sample(N);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < N; i++)
        S->insert(sample[i]);

    for (int i = 0; i < N; i++)
    {
        int doFault = S->remove(sample[i]);
        if (doFault)
        {
            n_fault++;

            // recovery query
            for (int j = i + 1; j < N; j++)
                S->insert(sample[j]);
        }
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("DMH, %d, %d, %u, %d, %f\n", k, l, 2 * N, n_fault, t);

    delete S;
    delete[] sample;
}

/**00, 15.401396
1024, 32, 131072, 1024, 0.20, 13.
 * @todo add documentation
 *
 * k: number of hash functions
 * l: size of the buffers
 * U: size of the universe
 * N: 2*N is the number of operations
 * max_size: size of the sliding window
 */
void slidingWindowMinHash(int k, int l, uint32_t U, int N, int max_size)
{
    TreeKLMinhash *S = new TreeKLMinhash(k, l, U, false);
    for (int j = 0; j < max_size; j++)
        S->insert(j);

    auto start = high_resolution_clock::now();
    int n_fault = 0;
    int first = 0;
    for (uint32_t i = 0; i < N; i++)
    {
        bool doFault = S->remove(first);
        if (doFault)
        {
            n_fault++;
            for (uint32_t j = first + 1; j < first + max_size; j++)
                S->insert(j);
        }
        S->insert(first + max_size + 1);
        first++;
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %d, %f\n", k, l, 2 * N, max_size, n_fault, t);
    delete S;
}

/**
 * @todo add documentation
 */
void testDSS(int c, int N)
{
    DSS *S = new DSS(c);
    uint32_t *sample = generate_random_sample(N);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < N; i++)
        S->insert(sample[i]);

    for (int i = 0; i < N; i++)
        S->remove(sample[i]);

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("DSS, %d, %d, %u, %f\n", c, S->k, 2 * N, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 */
void testDSSProactive(int c, int N, int n_hashes)
{

    DSSProactive *S = new DSSProactive(c, n_hashes);
    uint32_t *sample = generate_random_sample(N);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < N; i++)
        S->insert(sample[i]);

    for (int i = 0; i < N; i++)
        S->remove(sample[i]);

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("DSSp, %d, %d, %u, %f\n", c, S->k, 2 * N, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * WIP
 */
void testDSSQuery(int c, int size, int n_query, int n_hashes)
{
    DSS *S = new DSS(c, n_hashes);
    uint32_t *sample = generate_random_sample(size);

    for (int i = 0; i < size; i++)
        S->insert(sample[i]);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("DSS, %d, %d, %u, %u, %d, %f\n", c, S->k, size, n_query, n_hashes, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * WIP
 */
void testDSSProactiveQuery(int c, int size, int n_query, int n_hashes)
{
    DSSProactive *S = new DSSProactive(c, n_hashes);
    uint32_t *sample = generate_random_sample(size);

    for (int i = 0; i < size; i++)
        S->insert(sample[i]);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("DSSp, %d, %d, %u, %u, %d, %f\n", c, S->k, size, n_query, n_hashes, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * WIP
 */
void testKLMinhashQuery(int l, int size, int n_query, int n_hashes)
{
    TreeKLMinhash *S = new TreeKLMinhash(n_hashes, l, UINT32_MAX, false);
    uint32_t *sample = generate_random_sample(size);

    for (int i = 0; i < size; i++)
        S->insert(sample[i]);

    auto start = high_resolution_clock::now();
    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);

    float t = (float)duration.count() / 1000000.0;

    printf("DMH, %d, %d, %u, %u, %f\n", n_hashes, l, size, n_query, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a DSS sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testDSSUpdatesAndQuery(int c, int N, int n_hashes, float p, int start = 1)
{
    DSS *S = new DSS(c, n_hashes);
    uint32_t *sample = generate_random_sample(N + start);

    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    for (int i = 0; i < start; i++)
        S->insert(sample[i]);

    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(sample[i]);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->remove(sample[i]);
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("DSS, %d, %d, %u, %d, 0, %.2f, %f\n", c, S->k, 2 * N, n_hashes, p, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a DSSProactive sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testDSSProactiveUpdatesAndQuery(int c, int N, int n_hashes, float p, int start = 1)
{
    DSSProactive *S = new DSSProactive(c, n_hashes);
    uint32_t *sample = generate_random_sample(N + start);

    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    for (int i = 0; i < start; i++)
        S->insert(sample[i]);

    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(sample[i]);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->remove(sample[i]);
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("DSSp, %d, %d, %u, %d, 0, %.2f, %f\n", c, S->k, 2 * N, n_hashes, p, t);

    delete S;
    delete[] sample;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a L-buffered K-minhash sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testKLMinhashUpdatesAndQuery(int n_hashes, int l, int N, float p, int start = 1)
{
    TreeKLMinhash *S = new TreeKLMinhash(n_hashes, l, UINT32_MAX, false);
    uint32_t *sample = generate_random_sample(N + start);

    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    for (int i = 0; i < start; i++)
        S->insert(sample[i]);

    int n_fault = 0;
    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(sample[i]);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        int doFault = S->remove(sample[i]);
        if (doFault)
        {
            n_fault++;

            // recovery query
            for (int j = i + 1; j < N; j++)
                S->insert(sample[j]);
        }
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("DMH, %d, %d, %u, %d, %d, %.2f, %f\n", n_hashes, l, 2 * N, n_hashes, n_fault, p, t);

    delete S;
    delete[] sample;
}

double SE_DMH(int k, int l, uint32_t U, double p1, double p2, Hash<uint32_t> **hashes)
{
    TreeKLMinhash *SA = new TreeKLMinhash(k, l, UINT32_MAX, hashes, false);
    TreeKLMinhash *SB = new TreeKLMinhash(k, l, UINT32_MAX, hashes, false);

    __type *A = create(U, 0.05);
    __type *B = perturbate(A, U, p1, p2);

    for (int i = 0; i < U; i++)
    {
        if (get(A, i) == 1)
            SA->insert(i);
        if (get(B, i) == 1)
            SB->insert(i);
    }

    double estimation = TreeKLMinhash::similarity(SA, SB);
    double js = jaccard_sim(A, B, U);
    double err = estimation - js;

    delete SA;
    delete SB;
    delete[] A;
    delete[] B;

    return err * err;
}

double SE_DSS(int c, int k, uint32_t U, double p1, double p2, Hash<uint32_t> **hashes, Hash<uint32_t> *h1, Hash<uint32_t> *h2)
{
    DSS *SA = new DSS(c, h1, h2, hashes, k);
    DSS *SB = new DSS(c, h1, h2, hashes, k);

    __type *A = create(U, 0.05);
    __type *B = perturbate(A, U, p1, p2);

    for (int i = 0; i < U; i++)
    {
        if (get(A, i) == 1)
            SA->insert(i);
        if (get(B, i) == 1)
            SB->insert(i);
    }

    double alpha = .1;
    double r = .25;

    double estimation = DSS::similarity(SA, SB, alpha, r);
    double js = jaccard_sim(A, B, U);
    double err = estimation - js;

    delete SA;
    delete SB;
    delete[] A;
    delete[] B;

    return err * err;
}