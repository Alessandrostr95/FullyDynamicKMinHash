#include "../TreeKLMinhash.h"
#include "../DSS.cpp"
#include "../DSSProactive.cpp"
#include "../TreeBottomKL.h"
#include "../LSH.cpp"
#include <algorithm>
#include <chrono>
using namespace std::chrono;
using namespace std;

void permute(int *a, int n)
{
    random_shuffle(a, a + n);
}

/**
 * @todo add documentation
 */
void singleSetImplicit(int k, int l, uint32_t U, int N)
{
    int n_fault = 0;
    TreeKLMinhash *S = new TreeKLMinhash(k, l, U, false);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < N; i++)
        S->insert(i);

    for (int i = 0; i < N; i++)
    {
        int doFault = S->remove(i);
        if (doFault)
        {
            n_fault++;
            for (int j = i + 1; j < N; j++)
                S->insert(j);
        }
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %f\n", k, l, 2 * N, n_fault, t);

    delete S;
}

/**
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

    auto start = high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        S->insert(i);

    for (int i = 0; i < N; i++)
        S->remove(i);

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %f\n", c, S->k, 2 * N, t);

    delete S;
}

/**
 * @todo add documentation
 */
void testDSSProactive(int c, int N, int n_hashes)
{

    DSSProactive *S = new DSSProactive(c, n_hashes);

    auto start = high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        S->insert(i);

    for (int i = 0; i < N; i++)
        S->remove(i);

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %f\n", c, S->k, 2 * N, n_hashes, t);

    delete S;
}

/**
 * @todo add documentation
 * WIP
 */
void testDSSQuery(int c, int size, int n_query, int n_hashes)
{
    DSS *S = new DSS(c, n_hashes);

    for (int i = 0; i < size; i++)
        S->insert(i);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %u, %d, %f\n", c, S->k, size, n_query, n_hashes, t);

    delete S;
}

/**
 * @todo add documentation
 * WIP
 */
void testDSSProactiveQuery(int c, int size, int n_query, int n_hashes)
{
    DSSProactive *S = new DSSProactive(c, n_hashes);

    for (int i = 0; i < size; i++)
        S->insert(i);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %u, %d, %f\n", c, S->k, size, n_query, n_hashes, t);

    delete S;
}

/**
 * @todo add documentation
 * WIP
 */
void testKLMinhashQuery(int l, int size, int n_query, int n_hashes)
{
    TreeKLMinhash *S = new TreeKLMinhash(n_hashes, l, UINT32_MAX, false);

    for (int i = 0; i < size; i++)
        S->insert(i);

    auto start = high_resolution_clock::now();
    for (int i = 0; i < n_query; i++)
        S->getSignature();

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start);

    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %u, %f\n", n_hashes, l, size, n_query, t);

    delete S;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a DSS sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testDSSUpdatesAndQuery(int c, int N, int n_hashes, float p, int start = 1000)
{
    DSS *S = new DSS(c, n_hashes);
    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    // int start = N;
    for (int i = 0; i < start; i++)
        S->insert(i);

    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(i);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->remove(i);
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %.2f, %f\n", c, S->k, 2 * N, n_hashes, p, t);

    delete S;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a DSSProactive sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testDSSProactiveUpdatesAndQuery(int c, int N, int n_hashes, float p, int start = 1000)
{
    DSSProactive *S = new DSSProactive(c, n_hashes);
    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    // int start = N;
    for (int i = 0; i < start; i++)
        S->insert(i);

    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(i);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->remove(i);
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %.2f, %f\n", c, S->k, 2 * N, n_hashes, p, t);

    delete S;
}

/**
 * @todo add documentation
 * Exectute N insertions and N deletions in a L-buffered K-minhash sketch.
 * But a fraction `p` of the insertions and deletions are not executed.
 * Instead of that, a query is executed.
 */
void testKLMinhashUpdatesAndQuery(int n_hashes, int l, uint32_t U, int N, float p, int start = 1000)
{
    TreeKLMinhash *S = new TreeKLMinhash(n_hashes, l, U, false);
    int n_query = (int)(1 / p);

    N = N - (int)p * N;

    // int start = N;
    for (int i = 0; i < start; i++)
        S->insert(i);

    int n_fault = 0;
    auto start_time = high_resolution_clock::now();

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        S->insert(i);
    }

    for (int i = start; i < N + start; i++)
    {
        if (i % n_query == 0)
            S->getSignature();

        int doFault = S->remove(i);
        if (doFault)
        {
            n_fault++;
            for (int j = i + 1; j < N; j++)
                S->insert(j);
        }
    }

    auto duration = duration_cast<microseconds>(high_resolution_clock::now() - start_time);
    float t = (float)duration.count() / 1000000.0;

    printf("%d, %d, %u, %d, %.2f, %f\n", n_hashes, l, 2 * N, n_fault, p, t);

    delete S;
}