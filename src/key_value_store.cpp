#include "../include/key_value_store.h"
#include <algorithm>

KeyValueStore::KeyValueStore(int size) : maxSize(size) {
    // Initialize with the provided max size
}

void KeyValueStore::updateLRU(const std::string &key) {
    // Move the accessed key to the front of the LRU list
    usageOrder.erase(store[key].second);
    usageOrder.push_front(key);
    store[key].second = usageOrder.begin();
}

void KeyValueStore::evictLRU() {
    if (!usageOrder.empty()) {
        // Remove the least recently used key
        std::string lruKey = usageOrder.back();
        store.erase(lruKey);
        usageOrder.pop_back();
    }
}

void KeyValueStore::put(const std::string &key, const std::string &value) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    // Check if key already exists
    auto it = store.find(key);
    if (it != store.end()) {
        // Update existing key
        it->second.first = value;
        updateLRU(key);
    } else {
        // Check if we need to evict
        if (store.size() >= static_cast<size_t>(maxSize)) {
            evictLRU();
        }
        
        // Insert new key-value pair
        usageOrder.push_front(key);
        store[key] = std::make_pair(value, usageOrder.begin());
    }
}

std::string KeyValueStore::get(const std::string &key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    auto it = store.find(key);
    if (it != store.end()) {
        // Update LRU and return value
        updateLRU(key);
        return it->second.first;
    }
    
    // Key not found
    return "";
}

bool KeyValueStore::exists(const std::string &key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    return store.find(key) != store.end();
}

void KeyValueStore::del(const std::string &key) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    auto it = store.find(key);
    if (it != store.end()) {
        // Remove from LRU list
        usageOrder.erase(it->second.second);
        // Remove from store
        store.erase(it);
    }
}

bool KeyValueStore::saveToDisk(const std::string &filename) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    std::ofstream outFile(filename, std::ios::out);
    if (!outFile.is_open()) {
        return false;
    }
    
    // Write number of entries first
    outFile << store.size() << std::endl;
    
    // Write all key-value pairs
    for (const auto &key : usageOrder) {
        outFile << key << "\n" << store[key].first << std::endl;
    }
    
    outFile.close();
    return true;
}

bool KeyValueStore::loadFromDisk(const std::string &filename) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    std::ifstream inFile(filename, std::ios::in);
    if (!inFile.is_open()) {
        return false;
    }
    
    // Clear current store
    store.clear();
    usageOrder.clear();
    
    // Read number of entries
    size_t count;
    inFile >> count;
    inFile.ignore(); // Skip newline
    
    // Read all key-value pairs
    for (size_t i = 0; i < count; ++i) {
        std::string key, value;
        std::getline(inFile, key);
        std::getline(inFile, value);
        
        // Insert into store (without checking capacity since we're restoring)
        usageOrder.push_front(key);
        store[key] = std::make_pair(value, usageOrder.begin());
    }
    
    inFile.close();
    return true;
}

std::vector<std::string> KeyValueStore::getKeysWithPrefix(const std::string &prefix) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    std::vector<std::string> results;
    for (const auto &pair : store) {
        if (pair.first.substr(0, prefix.length()) == prefix) {
            results.push_back(pair.first);
        }
    }
    
    return results;
}

size_t KeyValueStore::size() const {
    return store.size();
}

void KeyValueStore::clear() {
    std::lock_guard<std::mutex> lock(storeMutex);
    store.clear();
    usageOrder.clear();
}

bool KeyValueStore::batchProcess(const std::vector<std::pair<std::string, std::pair<std::string, std::string>>> &operations) {
    std::lock_guard<std::mutex> lock(storeMutex);
    
    // Batch operations should be atomic, so we'll validate all operations first
    for (const auto &op : operations) {
        const std::string &operation = op.first;
        const std::string &key = op.second.first;
        
        if (operation == "get" && !exists(key)) {
            // If we're trying to get a non-existent key, fail the batch
            return false;
        }
    }
    
    // Now process all operations
    for (const auto &op : operations) {
        const std::string &operation = op.first;
        const std::string &key = op.second.first;
        const std::string &value = op.second.second;
        
        if (operation == "put") {
            // For put operations, we directly manipulate the store to avoid double-locking
            auto it = store.find(key);
            if (it != store.end()) {
                // Update existing key
                it->second.first = value;
                usageOrder.erase(it->second.second);
                usageOrder.push_front(key);
                it->second.second = usageOrder.begin();
            } else {
                // Check if we need to evict
                if (store.size() >= static_cast<size_t>(maxSize)) {
                    std::string lruKey = usageOrder.back();
                    store.erase(lruKey);
                    usageOrder.pop_back();
                }
                
                // Insert new key-value pair
                usageOrder.push_front(key);
                store[key] = std::make_pair(value, usageOrder.begin());
            }
        } else if (operation == "delete") {
            auto it = store.find(key);
            if (it != store.end()) {
                // Remove from LRU list
                usageOrder.erase(it->second.second);
                // Remove from store
                store.erase(it);
            }
        }
        // "get" operations don't need to be processed in batch mode
    }
    
    return true;
}