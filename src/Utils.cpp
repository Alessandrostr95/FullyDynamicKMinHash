#include <cstdio>
#include <cstdlib>

#include <string>
#include <iostream>
#include <random>
#include <fstream>

set<int> intersection(set<int> *s1, set<int> *s2)
{
    set<int> intersect;
    set_intersection(s1->begin(), s1->end(), s2->begin(), s2->end(), inserter(intersect, intersect.begin()));

    return intersect;
}

double jaccard(set<int> *s1, set<int> *s2)
{
    double size_s1 = s1->size();
    double size_s2 = s2->size();

    set<int> intersect = intersection(s1, s2);

    double size_in = intersect.size();

    double J = size_in / (size_s1 + size_s2 - size_in);

    return J;
}

std::unordered_map<int, set<int> *> *loadSetsFromFile(std::string fileName)
{
    std::ifstream file(fileName);

    std::string line;

    std::unordered_map<int, set<int> *> *sets = new std::unordered_map<int, set<int> *>();

    while (std::getline(file, line, '\n'))
    {
        std::string el;
        std::istringstream iss(line);

        std::getline(iss, el, ' ');
        int setId = stoi(el);

        sets->insert({setId, new set<int>()});

        while (std::getline(iss, el, ' '))
        {
            int x = stoi(el);
            sets->find(setId)->second->insert(x);
        }
    }

    return sets;
}