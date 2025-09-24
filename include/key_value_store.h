#ifndef KEY_VALUE_STORE_H
#define KEY_VALUE_STORE_H

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <mutex>

class KeyValueStore {
private:
    int maxSize;
    std::list<std::string> usageOrder; // LRU list
    std::unordered_map<std::string, std::pair<std::string, std::list<std::string>::iterator>> store;
    std::mutex storeMutex; // For thread-safe operations

    // Helper methods
    void updateLRU(const std::string &key);
    void evictLRU();

public:
    KeyValueStore(int size = 1000); // constructor
    
    // Basic CRUD operations
    void put(const std::string &key, const std::string &value);
    std::string get(const std::string &key);
    bool exists(const std::string &key);
    void del(const std::string &key);
    
    // Persistence
    bool saveToDisk(const std::string &filename = "store.txt");
    bool loadFromDisk(const std::string &filename = "store.txt");
    
    // Advanced features
    std::vector<std::string> getKeysWithPrefix(const std::string &prefix);
    size_t size() const;
    void clear();
    
    // Batch operations
    bool batchProcess(const std::vector<std::pair<std::string, std::pair<std::string, std::string>>> &operations);
};

#endif // KEY_VALUE_STORE_H