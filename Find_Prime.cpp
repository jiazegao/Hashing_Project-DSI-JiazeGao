
#include "Find_Prime.h"

std::vector<ull> getPrimes() {

    // Retrieve current primes
    std::ifstream primeFile("Prime.txt");
    std::vector<ull> primes;

    ull num;
    while (primeFile >> num) {
        primes.push_back(num);
    }

    primeFile.close();

    return primes;
}

void findPrime(ull target) {
    std::vector<ull> newPrimes;
    std::vector<ull> primes = getPrimes();

    // Start finding primes
    ull currNum = *(primes.end()-1)+1;
    while (currNum <= target) {
        bool valid = true;
        for (ull i = 0; primes[i]*primes[i] <= currNum; i++) {
            if (currNum % primes[i] == 0) {
                valid = false;
                break;
            }
        }
        if (valid) {
            primes.push_back(currNum);
            newPrimes.push_back(currNum);
        }
        currNum++;
    }

    std::ofstream primeOut("Prime.txt", std::ios::app);

    for (ull newPrime : newPrimes) {
        primeOut << newPrime << "\n";
    }

    primeOut.close();
}

void generateTableSizes(double base) {
    std::vector<ull> primes = getPrimes();
    std::vector<ull> tableSizes = {7};

    for (ull prime : primes) {
        if (prime > base*tableSizes[tableSizes.size()-1]) {
            tableSizes.push_back(prime);
        }
    }

    std::ofstream primeOut("TableSizes.txt");

    for (ull size : tableSizes) {
        primeOut << size << "\n";
    }

    primeOut.close();
}

std::vector<ull> getTableSizes() {
    // Retrieve current primes
    std::ifstream primeFile("TableSizes.txt");
    std::vector<ull> primes;

    ull num;
    while (primeFile >> num) {
        primes.push_back(num);
    }

    primeFile.close();

    return primes;
}
