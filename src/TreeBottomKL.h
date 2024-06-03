#ifndef TREEBOTTOMKL_H
#define TREEBOTTOMKL_H

#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <unordered_set>
#include <bits/stdc++.h>
#include "hash.cpp"
#include "Sketch.cpp"

using namespace std;

#define num uint32_t
#define NUM_MAX UINT32_MAX

#define P32 4294966297
#define P64 18446744073709550671

class TreeBottomKL : public Sketch
{
public:
    /**
     * U: the maximum size of the set (aka the universe size)
     */
    num U;

    /**
     * k: the number of the k of the bottom-k signature
     */
    int k;

    /**
     * l: the multiplicator of the k for the buffering (we have k*l elements in the buffer instead of k)
     */
    int l;

    /**
     * buffer: the buffer of the sketch
     */
    multiset<num> buffer;

    /**
     * Size: the current buffer size, exluding NUM_MAX elements
     */
    int size = 0;

    /**
     * delta: the maximum value in the buffer
     */
    num delta;

    /**
     * hashes: the pairwise hash function
     */
    Hash<num> *h;

    /**
     * signature: a pre-allocated array to store the bottom-k signature
     */
    num *signature;

    /**
     * explicitSet: if true, the set is explicitly stored
     */
    bool explicitSet;

    /**
     * elements: the set
     */
    std::unordered_set<num> elements;

public:
    TreeBottomKL() : k(1), l(1), U(1) {}

    TreeBottomKL(int k, int l, num U, Hash<num> *h, bool explicitSet = true)
        : k(k), l(l), U(U), h(h), explicitSet(explicitSet)
    {
        this->buffer = std::multiset<num>();
        this->delta = NUM_MAX;

        for (int i = 0; i < k * l; i++)
            this->buffer.insert(NUM_MAX);

        this->signature = (num *)malloc(k * sizeof(num));
    }

    /**
     * Constructor
     */
    TreeBottomKL(int k, int l, num U, bool explicitSet = true)
    {
        this->h = new PairWiseHash<num>(U);
        new (this) TreeBottomKL(k, l, U, h, explicitSet);
    }

    ~TreeBottomKL() {
        delete this->h;
        delete this->signature;
    }

    void insert(num x)
    {
        this->insert(x, true);
    }

    /**
     * Inserts x into the sketch.
     * If insertIntoSet is true (default) and isExplicitSet flag is true, x is also explicitaly stored.
     */
    void insert(num x, bool insertIntoSet)
    {
        if (this->explicitSet && insertIntoSet)
            this->elements.insert(x);

        num hx = (*this->h)(x);
        if (hx > this->delta)
            return;

        auto current_max = this->buffer.rbegin();

        this->size += (*next(current_max).base() == NUM_MAX);
        this->buffer.erase(next(current_max).base());
        this->buffer.insert(hx);

        num max = *this->buffer.rbegin();
        if (max < this->delta)
            this->delta = max;
    }

    /**
     * Removes x from the sketch.
     * If a fault occurs:
     * - the method returns true
     * - the buffer is reset
     * - if the explicitSet flag is true, all the elements are reinserted
     * The method returns false otherwise
     */
    bool remove(num x)
    {
        if (this->explicitSet)
            this->elements.erase(x);

        num hx = (*this->h)(x);
        if (hx > this->delta)
            return false;

        auto element = this->buffer.find(hx); 
        if (element != this->buffer.end())
        {
            this->buffer.erase(element);
            this->buffer.insert(NUM_MAX);
            this->size--;

            if (this->size == this->k)
            {
                this->resetBuffer();
                if (this->explicitSet)
                    this->fault();
                return true;
            }
        }

        return false;
    }

    /**
     * Reinserts all the elements in the sketch.
     */
    void fault()
    {
        auto itr = this->elements.begin();
        for (; itr != this->elements.end(); ++itr)
            this->insert(*itr, false);
    }

    /**
     * Resets the buffer to default values.
     */
    void resetBuffer()
    {
        this->delta = NUM_MAX;
        this->buffer.clear();
        this->size = 0;
        for (int i = 0; i < this->k * this->l; i++)
            this->buffer.insert(NUM_MAX);
    }

    /**
     * Returns the bottom-k signature of the set.
     */
    num *getSignature()
    {
        auto itr = this->buffer.begin();
        for (int i = 0; i < this->k; i++, itr++)
            signature[i] = *itr;
        return signature;
    }

    /**
     * Given two sketches A & B (TreeBottomKL), returns the estimation of their jaccard similarity using the bottom-k signature.
     */
    static float similarity(TreeBottomKL *A, TreeBottomKL *B)
    {
        num *sigA = A->getSignature();
        num *sigB = B->getSignature();

        // compute the size of the intersection
        int k = A->k;
        float intersection_size = .0;
        int i = 0, j = 0;
        while (i < k && j < k)
        {
            if (sigA[i] == sigB[j])
            {
                intersection_size++;
                i++;
                j++;
            }
            else if (sigA[i] < sigB[j])
                i++;
            else
                j++;
        }

        return intersection_size / (2.0 * k - intersection_size);
    }

    void print()
    {
        num current_max = this->delta;

        string start;
        string end;

        if (current_max == NUM_MAX)
            cout << "[∞]\t";
        else
            cout << "[" << current_max << "]\t";

        int kk = 0;
        for (auto itr = this->buffer.begin(); itr != this->buffer.end(); itr++)
        {
            if (*itr == NUM_MAX)
                cout << start << "∞" << end << " ";
            else
                cout << start << *itr << end << " ";

            if (++kk % k == 0)
                cout << "| ";
        }

        cout << "\t size = " << this->size << endl;
    }
};

#endif
