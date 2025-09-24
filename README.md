# Mini In-Memory Key-Value Store

A high-performance, in-memory key-value store implemented in C++ that simulates core database functionalities. This project demonstrates important computer science concepts including data structures, memory management, caching algorithms, and concurrent programming.

## Features

- **Basic CRUD Operations**
  - `put(key, value)`: Insert or update a key-value pair
  - `get(key)`: Retrieve a value by key
  - `del(key)`: Remove a key-value pair
  - `exists(key)`: Check if a key exists

- **LRU Cache Eviction**
  - Configurable maximum capacity
  - Least Recently Used (LRU) eviction policy
  - Efficient tracking using a linked list

- **Data Persistence**
  - Save the store to disk
  - Restore the store from disk
  - Automatic format handling

- **Advanced Features**
  - Prefix search functionality
  - Thread-safe operations with mutex protection
  - Batch operations with atomic execution

- **Performance Optimized**
  - O(1) average time complexity for most operations
  - Efficient memory usage
  - Smart pointer management

## Project Structure
├── include/ │ └── key_value_store.h # Class definition ├── src/ │ ├── key_value_store.cpp # Implementation │ └── main.cpp # Demo application ├── build/ # Build directory └── CMakeLists.txt # CMake build configuration


## Build Instructions

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher

### Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build . --config Release

Running the Application
# From the build directory
./Release/KeyValueStore.exe  # Windows
./KeyValueStore              # Linux/macOS
```
### Usage Examples
#### Basic Operations

// Create a store with capacity of 100 items
KeyValueStore store(100);

// Store values
store.put("user1", "John Doe");
store.put("user2", "Jane Smith");

// Retrieve values
std::string name = store.get("user1");  // "John Doe"

// Delete values
store.del("user2");

#### Prefix Search
// Find all keys with a given prefix
std::vector<std::string> keys = store.getKeysWithPrefix("user");

#### Persistence
// Save to disk
store.saveToDisk("store_backup.txt");

// Load from disk
KeyValueStore loadedStore(100);
loadedStore.loadFromDisk("store_backup.txt");

#### Batch Operations
std::vector<std::pair<std::string, std::pair<std::string, std::string>>> batchOps = {
    {"put", {"batch1", "Batch Value 1"}},
    {"put", {"batch2", "Batch Value 2"}},
    {"delete", {"user1", ""}}
};

store.batchProcess(batchOps);


#### Implementation Details
Uses std::unordered_map for O(1) average access time
Combines with std::list to implement LRU tracking
Uses std::mutex for thread safety
Implements modern C++ practices and RAII principles
Potential Extensions
Custom serialization formats
Multiple eviction policies (LFU, FIFO)
Network interface layer
Compression for stored values
TTL (Time-To-Live) for keys
Transaction support

### Author
Arnav Raj
