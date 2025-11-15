#pragma once
#include <fstream>
#include <iostream>
#include <vector>

typedef unsigned long long ull;

constexpr ull MAX_PRIME = 10000000;
constexpr double MULTIPLIER = 1.5;

std::vector<ull> getPrimes();
void findPrime(ull target = MAX_PRIME);
void generateTableSizes(double base = MULTIPLIER);
std::vector<ull> getTableSizes();