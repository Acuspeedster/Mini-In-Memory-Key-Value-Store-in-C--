#include "../include/key_value_store.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// Helper function to demonstrate multi-threaded operations
void threadFunction(KeyValueStore& store, int id, int operations) {
    for (int i = 0; i < operations; ++i) {
        std::string key = "thread_" + std::to_string(id) + "_key_" + std::to_string(i);
        std::string value = "value_" + std::to_string(i);
        
        // Perform operations
        store.put(key, value);
        std::string retrieved = store.get(key);
        if (retrieved != value) {
            std::cout << "Thread " << id << ": Value mismatch for key " << key << std::endl;
        }
        
        // Delete some keys
        if (i % 3 == 0) {
            store.del(key);
        }
    }
}

int main() {
    // Create a key-value store with capacity for 100 items
    KeyValueStore store(100);
    
    // Basic CRUD operations
    std::cout << "Demonstrating basic CRUD operations:" << std::endl;
    
    // Put operations
    store.put("user1", "John Doe");
    store.put("user2", "Jane Smith");
    store.put("user3", "Bob Johnson");
    
    // Get operation
    std::cout << "user1: " << store.get("user1") << std::endl;
    std::cout << "user2: " << store.get("user2") << std::endl;
    std::cout << "nonexistent: " << store.get("nonexistent") << std::endl;
    
    // Delete operation
    store.del("user2");
    std::cout << "After deleting user2: " << store.get("user2") << std::endl;
    
    // Prefix search
    std::cout << "\nDemonstrating prefix search:" << std::endl;
    store.put("user23", "Emily Davis");
    store.put("user45", "Michael Brown");
    
    std::vector<std::string> userKeys = store.getKeysWithPrefix("user");
    std::cout << "Keys with prefix 'user':" << std::endl;
    for (const auto& key : userKeys) {
        std::cout << "- " << key << ": " << store.get(key) << std::endl;
    }
    
    // Persistence
    std::cout << "\nDemonstrating persistence:" << std::endl;
    if (store.saveToDisk("store_backup.txt")) {
        std::cout << "Store saved to disk successfully." << std::endl;
    }
    
    // Create a new store and load from disk
    KeyValueStore loadedStore(100);
    if (loadedStore.loadFromDisk("store_backup.txt")) {
        std::cout << "Store loaded from disk successfully." << std::endl;
        std::cout << "Loaded user1: " << loadedStore.get("user1") << std::endl;
    }
    
    // Batch operations
    std::cout << "\nDemonstrating batch operations:" << std::endl;
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> batchOps = {
        {"put", {"batch1", "Batch Value 1"}},
        {"put", {"batch2", "Batch Value 2"}},
        {"delete", {"user1", ""}}
    };
    
    if (store.batchProcess(batchOps)) {
        std::cout << "Batch operations completed successfully." << std::endl;
        std::cout << "batch1: " << store.get("batch1") << std::endl;
        std::cout << "batch2: " << store.get("batch2") << std::endl;
        std::cout << "user1 (should be deleted): " << (store.get("user1").empty() ? "deleted" : "still exists") << std::endl;
    }
    
    // Multi-threading demonstration
    std::cout << "\nDemonstrating multi-threading with 5 threads:" << std::endl;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread(threadFunction, std::ref(store), i, 50));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Multi-threading test completed. Final store size: " << store.size() << std::endl;
    
    // Demonstrate LRU eviction
    std::cout << "\nDemonstrating LRU eviction:" << std::endl;
    KeyValueStore smallStore(5); // Store with capacity for only 5 items
    
    for (int i = 1; i <= 7; ++i) {
        std::string key = "lru_key_" + std::to_string(i);
        smallStore.put(key, "value_" + std::to_string(i));
        std::cout << "Added " << key << std::endl;
    }
    
    // The first two items should be evicted due to LRU policy
    std::cout << "lru_key_1 (should be evicted): " 
              << (smallStore.get("lru_key_1").empty() ? "evicted" : "still exists") << std::endl;
    std::cout << "lru_key_2 (should be evicted): " 
              << (smallStore.get("lru_key_2").empty() ? "evicted" : "still exists") << std::endl;
    std::cout << "lru_key_7 (should exist): " 
              << (smallStore.get("lru_key_7").empty() ? "evicted" : "still exists") << std::endl;
    
    std::cout << "\nKey-Value Store demo completed successfully!" << std::endl;
    
    return 0;
}