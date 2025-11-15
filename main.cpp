#include <bitset>
#include <iostream>
#include <vector>
#include <random>

#include "Find_Prime.h"

typedef unsigned long long ull;
constexpr ull MAX_HASH = INT_MAX;

// RNG Setup
std::mt19937 engine(std::random_device{}());
std::uniform_int_distribution<int> RNG(1, MAX_HASH-1);
std::uniform_int_distribution<int> RNG_SMALL(1, 8191);
std::uniform_int_distribution<int> RNG_MID(1, 1000000);

// Worksheet Verification
std::vector<ull> workSheetItems = {7198, 841, 6273, 2172, 1876, 305, 7146, 5480, 6989, 3725, 4932, 7780};
ull generalPreset[4] = {1, 792, 158, 8191};
ull cuckooPreset[4] = {1, 37, 39, 8191};
ull perfectPreset[4] = {0, 1, 2, 8191};

// Test
std::vector<ull> integerSequence = {563, 6473, 785, 4832, 2490, 6667, 3249, 3839, 708, 4309, 1175, 2618, 4993, 717, 1065, 3208, 8100, 6223, 6408, 149, 5606, 5141, 7892, 1784, 3303, 1668, 6252, 6828, 3272, 7498, 3989, 5061, 2794, 3692, 739, 1483, 1479, 6789, 8054, 2201};
ull testPreset1[4] = {5, 792, 158, 8191};
ull testPreset2[4] = {0, 98, 34, 8191};
ull testPreset3[4] = {5, 234, 531, 8191};
ull testPreset4[4] = {3, 233, 654, 8191};

std::vector<ull> TABLE_SIZES = getTableSizes();
const ull MAX_HOPSCOTCH_SHIFT = 4;

template <typename T>
class Hash {
public:
    explicit Hash(const ull sizeCount, const ull a, const ull b, const ull p) {
        hashTable = std::vector<T>(TABLE_SIZES[sizeCount]);
        this->a = a;
        this->b = b;
        this->p = p;
        this->currSize = 0;
        this->sizeCount = sizeCount;
    }

    explicit Hash(const ull sizeCount) {
        hashTable = std::vector<T>(TABLE_SIZES[sizeCount]);
        this->a = RNG(engine);
        this->b = RNG(engine);
        this->p = MAX_HASH;
        this->currSize = 0;
        this->sizeCount = sizeCount;
    }

    explicit Hash(const ull preSet[4]) {
        hashTable = std::vector<T>(TABLE_SIZES[preSet[0]]);
        this->a = preSet[1];
        this->b = preSet[2];
        this->p = preSet[3];
        this->currSize = 0;
        this->sizeCount = preSet[0];
    }

    bool universal_hash (const T k) {
        if (k < MAX_HASH) {
            ull index = (a*k+b)%p%hashTable.size();
            if (hashTable[index] == 0) {
                hashTable[index] = k;
                currSize++;
                return true;
            }
            return false;
        }
        return false;
    }

    bool exist (const T k) {
        return hashTable[(a*k+b)%p%hashTable.size()] != 0;
    }

    bool remove (const T k) {
        if (exist(k)) {
            hashTable[getIndex(k)] = 0;
            currSize--;
            return true;
        }
        return false;
    }

    bool pop (ull index) {
        if (index >= hashTable.size() || hashTable[index] == 0) return false;
        hashTable[index] = 0;
        currSize--;
        return true;
    }

    bool rehash (const ull sc, const ull a, const ull b, const ull p) {

        if (sc < sizeCount) return false;

        // Create new hashTable
        Hash<T> newHashTable(sc, a, b, p);

        //std::cout << this->getABSSize() << " " << this->getCurrSize() << std::endl;

        // Rehash
        for (T x: hashTable) {
            if (x==0) continue;
            // If collided, return false
            if (!newHashTable.universal_hash(x)) return false;
        }

        // Reassign constants
        this->a = newHashTable.a;
        this->b = newHashTable.b;
        this->p = newHashTable.p;
        hashTable = newHashTable.hashTable;
        this->sizeCount = newHashTable.sizeCount;
        this->currSize = newHashTable.currSize;

        return true;
    }

    void rehash (const ull sc) {
        while (!rehash(sc, RNG(engine), RNG(engine), this->p)) {}
    }

    void layout (bool indent = false) {
        for (int i = 0; i < hashTable.size(); i++) {
            if (indent) std::cout << "\t";
            std::cout << i << " " << hashTable[i] << std::endl;
        }
        std::cout << "\tItems: " << currSize << std::endl;
    }

    ull getIndex (const T k) {
        return (a*k+b)%p%hashTable.size();
    }

    ull getABSSize() {
        return hashTable.size();
    }

    T get(const ull index) {
        if (index >= hashTable.size() || hashTable[index] == 0) return 0;
        return hashTable[index];
    }

    [[nodiscard]] ull getCurrSize() const {
        return currSize;
    }

    Hash& operator=(const Hash<T> target) {
        if (this == &target) return *this;

        hashTable = target.hashTable;
        a = target.a;
        b = target.b;
        p = target.p;
        currSize = target.currSize;
        sizeCount = target.sizeCount;
        return *this;
    }

    [[nodiscard]] ull getSizeCount() const {
        return sizeCount;
    }

    std::vector<T> hashTable;
    ull a, b, p, currSize, sizeCount;
};

template <typename T>
class Perfect_Hash {
public:
    explicit Perfect_Hash (std::vector<T> items, ull sizeCount = 0) {
        // Init values
        this->a = RNG(engine);
        this->b = RNG(engine);
        this->p = MAX_HASH;
        this->sizeCount = sizeCount;
        // Init hash tables
        hashTables = std::vector<Hash<T>>();
        for (int i = 0; i < TABLE_SIZES[sizeCount]; i++) {
            hashTables.push_back(Hash<T>(sizeCount));
        }
        // Hash all items
        for (int i = 0; i < items.size(); i++) {
            perfect_hash(items[i]);
        }
    }

    explicit Perfect_Hash (std::vector<T> items, ull const generalPS[4], const ull subPS[4]) {
        // Init values
        this->a = generalPS[1];
        this->b = generalPS[2];
        this->p = generalPS[3];
        this->sizeCount = generalPS[0];
        // Init hash tables
        hashTables = std::vector<Hash<T>>();
        for (int i = 0; i < TABLE_SIZES[generalPS[0]]; i++) {
            hashTables.push_back(Hash<T>(subPS));
        }
        // Hash all items
        for (int i = 0; i < items.size(); i++) {
            perfect_hash(items[i]);
        }
    }

    bool perfect_hash (const T k) {
        if (k < MAX_HASH) {
            ull index = (a*k+b)%p%hashTables.size();
            // If the same value, exit
            if (hashTables[index].get(hashTables[index].getIndex(k)) == k) return false;
            while (!hashTables[index].universal_hash(k)) {
                // Expand the table if it's too small
                if (hashTables[index].getSizeCount()+1 < TABLE_SIZES.size() && std::pow(hashTables[index].getCurrSize(), 2) > hashTables[index].getABSSize()) {
                    hashTables[index].rehash(hashTables[index].getSizeCount()+1);
                }
                else {
                    hashTables[index].rehash(hashTables[index].getSizeCount());
                }
            }
            return true;
        }
        return false;
    }

    bool exist (const T k) {
        return hashTables[(a*k+b)%p%hashTables.size()].exist(k) != 0;
    }

    void layout () {
        ull sum = 0;
        ull total = 0;
        std::cout << "Perfect Hash Layout: " << std::endl;

        ull remaining = hashTables.size();
        ull start = 0;
        ull end = remaining < 7 ? remaining : 7;
        ull max_size = 0;

        // Calculate sums
        for (ull i = 0; i < hashTables.size(); i++) {
            sum += hashTables[i].getCurrSize();
            total += hashTables[i].getABSSize();
        }

        // Layouts
        while (remaining > 0) {

            remaining -= remaining >= 7 ? 7 : remaining ;

            max_size = 0;

            // Print headers
            std::cout << std::endl;
            std::cout << "Index\t";
            for (ull i = start; i < end; i++) {
                std::cout << i << "\t" ;
            }
            std::cout << std::endl << std::endl;

            // Find max
            for (ull i = start; i < end; i++) {
                max_size = std::max(max_size, hashTables[i].getABSSize());
            }

            // Print tables
            for (ull i = 0; i < max_size; i++) {
                std::cout << i << "\t";
                for (ull j = start; j < end; j++) {
                    std::cout << (i < hashTables[j].getABSSize() ? hashTables[j].hashTable[i] : 0) << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;

            // Repeat
            start = end;
            end += remaining < 7 ? remaining : 7;
        }

        // Print sums
        std::cout << "Total Items: " << sum << std::endl;
        std::cout << "Table Size: " << total << std::endl;
    }

    ull getABSSize () {
        ull sum = 0;
        for (int i = 0; i < hashTables.size(); i++) {
            sum += hashTables[i].getABSSize();
        }
        return sum;
    }

    ull getCurrSize () {
        ull sum = 0;
        for (int i = 0; i < hashTables.size(); i++) {
            sum += hashTables[i].getCurrSize();
        }
        return sum;
    }

private:
    std::vector<Hash<T>> hashTables;
    ull a, b, p, sizeCount;
};

template <typename T>
class Cuckoo_Hash {
public:
    explicit Cuckoo_Hash (ull sizeCount = 0, ull allowedIt = 10) {
        // Init hash tables
        hashTables = std::vector<Hash<T>>();
        hashTables.push_back(Hash<T>(sizeCount));
        hashTables.push_back(Hash<T>(sizeCount));
        this->allowedIt = allowedIt;
        this->sizeCount = sizeCount;
    }

    explicit Cuckoo_Hash (ull PS1[4], ull PS2[4], ull allowedIt = 10) {
        // Init hash tables
        hashTables = std::vector<Hash<T>>();
        hashTables.push_back(Hash<T>(PS1));
        hashTables.push_back(Hash<T>(PS2));
        this->allowedIt = allowedIt;
        this->sizeCount = PS1[0];
    }

    bool cuckoo_hash (T k) {
        bool onSecond = false;
        while (true) {
            for (int i = 0; i < allowedIt; i++) {
                ull index = hashTables[onSecond].getIndex(k);
                if (hashTables[onSecond].hashTable[index] != 0) {
                    T old_k = k;
                    k = hashTables[onSecond].get(index);
                    //std:: cout << k << " " << old_k << std::endl;
                    if (k == old_k) return false;
                    hashTables[onSecond].hashTable[index] = old_k;
                    onSecond = !onSecond;
                }
                else {
                    hashTables[onSecond].universal_hash(k);
                    return true;
                }
            }
            // Rehash
            if (sizeCount+1 < TABLE_SIZES.size() && !( std::pow(hashTables[0].getCurrSize(), 2) < hashTables[0].getABSSize() && std::pow(2*hashTables[1].getCurrSize(), 2) < hashTables[1].getABSSize() )) {
                sizeCount++;
            }
            hashTables[0].rehash(sizeCount);
            hashTables[1].rehash(sizeCount);
        }
    }

    bool exist (const T k) {
        if (hashTables[0].exist(k) || hashTables[1].exist(k)) return true;
        return false;
    }

    void layout() {
        std::cout << "Cuckoo Hash Layout: " << std::endl;
        // Print headers
        std::cout << "\nIndex\t\tTable 1\t\tTable 2\n";
        // Print tables
        for (int i = 0; i < TABLE_SIZES[sizeCount]; i++) {
            std::cout << i << "\t\t" << hashTables[0].hashTable[i] << "\t\t" << hashTables[1].hashTable[i] << std::endl;
        }
        // Print sums
        ull sum = hashTables[0].getCurrSize() + hashTables[1].getCurrSize();
        std::cout << "\nTotal Items: " << sum << std::endl;
        std::cout << "Table Size: 2 * " << TABLE_SIZES[sizeCount] << std::endl;
    }

private:
    std::vector<Hash<T>> hashTables;
    ull allowedIt, sizeCount;
};

template <typename T>
class Hopscotch_Hash {
public:

    explicit Hopscotch_Hash(const ull sizeCount) {
        hashTable = std::vector<T>(TABLE_SIZES[sizeCount]);
        bitSets = std::vector<std::bitset<MAX_HOPSCOTCH_SHIFT>>(TABLE_SIZES[sizeCount]);
        this->a = RNG(engine);
        this->b = RNG(engine);
        this->p = MAX_HASH;
        this->currSize = 0;
        this->sizeCount = sizeCount;
    }

    explicit Hopscotch_Hash(const ull preSet[4]) {
        hashTable = std::vector<T>(TABLE_SIZES[preSet[0]]);
        bitSets = std::vector<std::bitset<MAX_HOPSCOTCH_SHIFT>>(TABLE_SIZES[preSet[0]]);
        this->a = preSet[1];
        this->b = preSet[2];
        this->p = preSet[3];
        this->currSize = 0;
        this->sizeCount = preSet[0];
    }

    bool hopscotch_hash (const T k) {
        if (k < MAX_HASH) {

            while (true) {
                ull index = (a*k+b)%p%hashTable.size();

                // Normal linear probing
                for (int increment = 0; increment < MAX_HOPSCOTCH_SHIFT; increment++) {
                    if (hashTable[(index + increment)%hashTable.size()] == k ) return false;
                    if (hashTable[(index + increment)%hashTable.size()] == 0 ) {
                        hashTable[(index + increment)%hashTable.size()] = k;
                        bitSets[index][increment] = true;
                        currSize++;
                        return true;
                    }
                }

                // Attempt to find a potential hop
                for (int increment = 1; increment < MAX_HOPSCOTCH_SHIFT; increment++) {
                    ull currIndex = (index+increment)%TABLE_SIZES[sizeCount];
                    // Potential hop
                    if (bitSets[currIndex][0] == true) {
                        // Check if the following spaces are available
                        for (int add = 1; add < MAX_HOPSCOTCH_SHIFT; add++) {
                            ull currIndexAdd = (currIndex+add)%TABLE_SIZES[sizeCount];
                            // Opening
                            if (hashTable[currIndexAdd] == 0) {
                                // Adjust target info
                                hashTable[currIndexAdd] = hashTable[currIndex];
                                hashTable[currIndex] = 0;
                                bitSets[currIndex][0] = false;
                                bitSets[currIndex][add] = true;
                                // Replace k
                                hashTable[currIndex] = k;
                                bitSets[index][increment] = true;
                                currSize++;
                                return true;
                            }
                        }
                    }
                }

                // Last resort: Rehash
                if (sizeCount+1 < TABLE_SIZES.size() && currSize*2 > hashTable.size()) {
                    rehash(++sizeCount);
                }
                else {
                    rehash(sizeCount);
                }
            }

        }
        return false;
    }

    bool exist (const T k) {
        // Verify all potential locations
        ull index = (a*k+b)%p%hashTable.size();
        for (int i = 0; i < MAX_HOPSCOTCH_SHIFT; i++) {
            if (bitSets[index][i] == true && hashTable[(index+i)%hashTable.size()] == k) return true;
        }
        return false;
    }

    bool remove (const T k) {
        // Verify all potential locations
        ull index = (a*k+b)%p%hashTable.size();
        for (int i = 0; i < MAX_HOPSCOTCH_SHIFT; i++) {
            if (bitSets[index][i] == true && hashTable[(index+i)%hashTable.size()] == k) {
                bitSets[index][i] = false;
                hashTable[(index+i)%hashTable.size()] = 0;
                currSize--;
                return true;
            }
        }
        return false;
    }

    bool rehash (const ull sc) {

        if (sc < sizeCount) return false;

        // Create new hashTable
        Hopscotch_Hash<T> newHashTable(sc);

        // Rehash
        for (T x: hashTable) {
            if (x==0) continue;
            newHashTable.hopscotch_hash(x);
        }

        // Reassign constants
        this->a = newHashTable.a;
        this->b = newHashTable.b;
        this->p = newHashTable.p;
        hashTable = newHashTable.hashTable;
        bitSets = newHashTable.bitSets;
        this->sizeCount = newHashTable.sizeCount;
        this->currSize = newHashTable.currSize;

        return true;
    }

    void layout (bool indent = false) {
        std::cout << "Hopscotch Hash Layout:" << std::endl;
        // Print headers
        std::cout << "\nIndex\t\tHash Table\tHop Table\n";
        // Print tables
        for (int i = 0; i < hashTable.size(); i++) {
            if (indent) std::cout << "\t";
            std::cout << i << "\t\t" << hashTable[i] << "\t\t";
            for (int j = 0; j < MAX_HOPSCOTCH_SHIFT; j++) {
                std::cout << bitSets[i][j];
            }
            std::cout << std::endl;
        }
        // Print sums
        std::cout << "\nTotal Items: " << currSize << std::endl;
        std::cout << "Table Size: " << TABLE_SIZES[sizeCount] << std::endl;
    }

    ull getABSSize() {
        return hashTable.size();
    }

    [[nodiscard]] ull getCurrSize() const {
        return currSize;
    }

private:
    std::vector<T> hashTable;
    std::vector<std::bitset<MAX_HOPSCOTCH_SHIFT>> bitSets{};
    ull a, b, p, currSize, sizeCount;
};

// Worksheet terminal verification
void workSheetVerification () {
    std::cout << "--------------------- WORKSHEET VERIFICATION ---------------------" << std::endl;

    // Create hash tables
    Perfect_Hash<ull> pHash(workSheetItems, generalPreset, perfectPreset);
    Cuckoo_Hash<ull> cHash(generalPreset, cuckooPreset);
    Hopscotch_Hash<ull> hHash(generalPreset);

    // Insert items
    for (int i = 0; i < workSheetItems.size(); i++) {
        cHash.cuckoo_hash(workSheetItems[i]);
        hHash.hopscotch_hash(workSheetItems[i]);
    }

    // Print layouts
    pHash.layout();
    std::cout << std::endl;
    cHash.layout();
    std::cout << std::endl;
    hHash.layout();
}

// Random integers hashing
void randomSequenceHash () {

    std::cout << "----------------------- RANDOM SEQUENCE HASHING -----------------------" << std::endl;

    // Create hash tables
    Perfect_Hash<ull> pHash(integerSequence, testPreset4, testPreset2);
    Cuckoo_Hash<ull> cHash(testPreset1, testPreset3);
    Hopscotch_Hash<ull> hHash(testPreset1);

    // Insert items
    for (int i = 0; i < integerSequence.size(); i++) {
        cHash.cuckoo_hash(integerSequence[i]);
        hHash.hopscotch_hash(integerSequence[i]);
    }

    // Print layouts
    pHash.layout();
    std::cout << std::endl;
    cHash.layout();
    std::cout << std::endl;
    hHash.layout();
}

int main() {

    workSheetVerification();
    std::cout << std::endl;
    std::cout << std::endl;
    randomSequenceHash();

    return 0;
}