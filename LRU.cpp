/* A C++ function for the Least Recently Used Caching algorithm which takes user input for cache size and the paging sequence
and returns the final cache and also the number of cache misses. */

/* The algorithm mainly uses two data structures - 1) hashMaps (for the cache to reference to in case the page referenced is not present in the cache)
                                                   2) Queue (for the cache itself)
The reason we are using the hashMap is to access the address of the referenced page. (acts as psuedo main memory)*/

using namespace std;

#include <iostream>

// A Queue Node (Queue is implemented using Doubly Linked List)

typedef struct QNode {
    struct QNode *previous, *next;
    unsigned pageNumber;
} QNode;


typedef struct Queue {
    unsigned count; // How much of the cache is filled
    unsigned cacheSize; // total number of frames
    QNode *front, *rear;
} Queue;

// A hash (Collection of pointers to Queue Nodes)

typedef struct Hash {
    int capacity; // how many pages can be there
    QNode** nodes; // an array of queue nodes
} Hash;


// Utility Functions

// Function to create a new Queue Node which stores the given 'pageNumber'

QNode* newQNode(unsigned pageNumber) {

    // Allocate memory and assign 'pageNumber'
    QNode* temp = (QNode*)malloc(sizeof(QNode));
    temp->pageNumber = pageNumber;

    // Initialize previous and next as NULL
    temp->previous = temp->next = NULL;

    return temp;
}

// Function to create a cache of cacheSize = size (of given size)

Queue* createCache(int size) {

    // Allocating memory for the cache
    Queue* queue = (Queue*)malloc(sizeof(Queue));

    queue->count = 0;
    queue->front = queue->rear = NULL;

    // setting the cacheSize
    queue->cacheSize = size;

    return queue;
}

// Function to create an empty hashMap of given capacity, this will be used to as secondary storage for the memory sequence

Hash* createHashMap(int capacity) {

    // Allocating memory for the hashMap
    Hash* hash = (Hash*)malloc(sizeof(Hash));
    hash->capacity = capacity;

    // Creating an array of pointers for referring to queue nodes
    hash->nodes = (QNode**)malloc(hash->capacity * sizeof(QNode*));

    // Initialize all hash entries as empty initially
    for (int i = 0; i < hash->capacity; i++) {
        hash->nodes[i] = NULL;
    }

    return hash;
}

// Function to check if there is slot available in memory

int isCacheFull(Queue* queue) {
    return queue->count == queue->cacheSize;
}

// Function to check if queue is empty

int isQueueEmpty(Queue* queue) {
    return queue->rear == NULL;
}

// Queue operations

// deQueue operation to remove a pageNumber from the cache

void deQueue(Queue* queue) {

    // Check if queue is empty
    if (isQueueEmpty(queue)) return;

    // If this is the only node in list, then change front
    if (queue->front == queue->rear)
        queue->front = NULL;

    // Change rear and remove the previous rear
    QNode* temp = queue->rear;
    queue->rear = queue->rear->previous;

    if (queue->rear) {
        queue->rear->next = NULL;
    }

    free(temp);

    queue->count--;
}

// enQueue operation to add a pageNumber to both hash and queue

void enQueue(Queue* cache, Hash* hash, unsigned pageNumber) {

    // If all frames are full, remove the pageNumber at the rear
    if (isCacheFull(cache)) {
        // remove pageNumber from hash
        hash->nodes[cache->rear->pageNumber] = NULL;
        deQueue(cache);
    }

    // Create a new node with given pageNumber and add it to the front of the cache
    QNode* temp = newQNode(pageNumber);
    temp->next = cache->front;

    // If cache is empty change both front and rear pointers, else change the front
    if (isQueueEmpty(cache)) {
        cache->rear = cache->front = temp;

    } else {
        cache->front->previous = temp;
        cache->front = temp;
    }

    // And then add it to the hash
    hash->nodes[pageNumber] = temp;

    // Increment count to show that the cache has a filled slot now
    cache->count++;
}

// Function to pull the referenced page from the cache.

int referencePage(Queue* queue, Hash* hash, unsigned pageNumber) {
    QNode* requestedPageNumber = hash->nodes[pageNumber];

    // If require pageNumber is not in cache, pull it from hash
    if (requestedPageNumber == NULL) {
        enQueue(queue, hash, pageNumber);
        return 1; // In case of cache miss, it returns 1. We use a counter variable in the driver code to count the number of misses using this.

    // If required pageNumber is present in cache
    } else {

        // Unlink requested pageNumber from its current location in queue.
        requestedPageNumber->previous->next = requestedPageNumber->next;

        // If the pointer to the node next to our requestedPageNumber is not null
        if (requestedPageNumber->next) {
            requestedPageNumber->next->previous = requestedPageNumber->previous;
        }

        // If the requestedPageNumber is rear, then change rear as this node will be moved to front
        if (requestedPageNumber == queue->rear) {
            queue->rear = requestedPageNumber->previous;
            queue->rear->next = NULL;
        }

        // Put the requested pageNumber before current front
        requestedPageNumber->next = queue->front;
        requestedPageNumber->previous = NULL;

        requestedPageNumber->next->previous = requestedPageNumber;
        queue->front = requestedPageNumber;

        return 0;
    }
}

// Driver program to test above functions
int main() {

    int cacheSize, memorySequenceLength, mainMemorySize, cacheMisses = 0;

    cout << "Enter size of the cache: " << endl;
    cin >> cacheSize;
    cout << "Enter the size of the main memory: " << endl;
    cin >> mainMemorySize;
    cout << "Enter the length of the memory sequence: " << endl;
    cin >> memorySequenceLength;

    int memorySequence[memorySequenceLength], mainMemory[mainMemorySize];

    cout << "Enter your memory sequence in the order it has to be referenced:" << endl;

    Queue* cache = createCache(cacheSize);

    for (int i = 0; i < memorySequenceLength; i++) {
        cin >> memorySequence[i];
    }

    Hash* hash = createHashMap(memorySequenceLength);

    for (int i = 0; i < memorySequenceLength; i++) {
        cacheMisses += referencePage(cache, hash, memorySequence[i]);
    }

    // Printing the final contents of the cache
    cout << "Cache: | ";
    QNode* x = cache->front;
    for (int i = 0; i < cacheSize; i++) {
        cout << x->pageNumber << " | ";
        x = x->next;
    }

    cout << endl << "Total number of cache misses = " << cacheMisses << endl;

    return 0;
}
