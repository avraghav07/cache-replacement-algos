/* A C++ function for the Least Frequently Used Caching algorithm which takes user input for cache size and the paging sequence
and returns the final cache and also the number of cache misses. */

/* The algorithm mainly uses two data structures - 1) hashMaps (for the cache to reference to in case the page referenced is not present in the cache)
                                                   2) Priority Queue (for the cache itself, using frequency)
The reason we are using the hashMap is to access the address of the referenced page. (acts as psuedo main memory)*/

using namespace std;

#include <iostream>

// struct definiton of a Priority Queue, where the QNode stores the pageNumber and the frequency

// A Queue QNode (Queue is implemented using Doubly Linked List)

typedef struct QNode {
    struct QNode *previous, *next;
    unsigned pageNumber;
    unsigned frequency; //To check how many times the pageNumber has been referenced
} QNode;


typedef struct Queue {
    unsigned count; // Number of filled frames
    unsigned cacheSize; // total number of frames
    QNode *front, *rear;
} Queue;

// A hash (Collection of pointers to Queue Nodes)

typedef struct Hash {
    int capacity; // how many pages can be there
    QNode** nodes; // an array of queue nodes
} Hash;

// Utility Functions

// Function to create a new Queue QNode which stores the given 'pageNumber'

QNode* newQNode(unsigned pageNumber) {

    // Allocate memory and assign 'pageNumber', initial frequency as 0
    QNode* temp = (QNode*)malloc(sizeof(QNode));
    temp->pageNumber = pageNumber;
    temp->frequency = 0;
    // Initialize previous and next as NULL
    temp->previous = temp->next = NULL;

    return temp;
}

// Function to create an empty Queue which can have at most 'cacheSize' nodes

Queue* createQueue(int cacheSize) {

    // Allocate memory for queue (our cache)
    Queue* queue = (Queue*)malloc(sizeof(Queue));

    queue->count = 0;
    queue->front = queue->rear = NULL;

    // Number of frames that can be stored in memory
    queue->cacheSize = cacheSize;

    return queue;
}

// Function to create an empty hashMap of given capacity

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

// Function to calculate the index of the pageNumber with minimum frequency in the cache

int getMinFreqIndex(Queue* cache) {

    int minimum = 100000;
    int minFreqIndex=minimum;
    QNode* tempPointer = cache->front;


    // Error checking to be on the safe side

    // If there is nothing at the front of the cache, then error
    if (tempPointer == NULL) {
        return -1;
    }

    // If there is only one element in the cache, then error
    if (tempPointer->next == NULL) {
        return 0;
    }

    // Iterate through the cache and find the index of the pageNumber with the minimum frequency
    for (int i = 0; i < cache->cacheSize ;i++) {

        if (tempPointer->frequency < minimum) {
            minimum = tempPointer->frequency;
            minFreqIndex = i;
        }
        tempPointer = tempPointer->next;
    }

    return minFreqIndex;
}


// Function to return the pageNumber with minimum frequency

unsigned getMinPageNumber(Queue* queue) {

    int minFreqIndex = getMinFreqIndex(queue);

    // If cache was empty for some reason, return some weird value (error handling)
    if (minFreqIndex == -1) {
        return 69;
    }

    int indexCount = 0; // Temp loop counter
    QNode* tempPointer = queue->front;

    if (tempPointer == NULL) {
        //more queue empty bs
        return 69;
    }

    while (tempPointer->next != NULL) {
       if (indexCount == minFreqIndex) {
           return tempPointer->pageNumber;
       } else {
           indexCount++;
       }
    }
    // If it never found the min somehow, despite looping throughout the cache then return some weird value (more error handling)
    return 70;
}

// Queue operations

// Function to delete node at specified index

void deleteNode(struct QNode** cache, struct QNode* page) {

    // If cache is empty, then exit
    if (*cache == NULL || page == NULL)
        return;

    // If the head page has to be deleted
    if (*cache == page)
        *cache = page->next;

    // Change next only if page to be deleted is NOT the last page
    if (page->next != NULL)
        page->next->previous = page->previous;

    // Change previous only if page to be deleted is NOT the first page
    if (page->previous != NULL)
        page->previous->next = page->next;

    // Finally, free the memory occupied by page
    free(page);
}

void deleteLeastFrequentlyUsedPage(Queue* cache) {// struct QNode** node was original param

    int minFreqNodeIndex = getMinFreqIndex(cache);
    QNode** node = &(cache->front); // might be buggy
    struct QNode* current = *node;

    // Traversing to the page with minimum frequency, from the beginning
    for (int i = 0; current != NULL && i < minFreqNodeIndex; i++) {
        current = current->next;
    }
    // If the index of the minimum frequency page is greater than the number of nodes in the doubly linked list, then exit
    if (current == NULL)
        return;

    // Delete the QNode pointed to by 'current', which will be the page with the minimum frequency
    deleteNode(node, current);
}

// enQueue operation to add a pageNumber to both hash and queue

void enQueue(Queue* cache, Hash* hash, unsigned pageNumber) {

    // If cache is full, remove the least frequently used page
    if (isCacheFull(cache)) {

        // Getting the least frequently used page and removing it from the cache and hash aswell
        int minPageNumber = getMinPageNumber(cache);
        hash->nodes[minPageNumber] = NULL;
        deleteLeastFrequentlyUsedPage(cache);
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
    QNode* requestedPage = hash->nodes[pageNumber];

    // If require pageNumber is not in cache, pull it from hash
    if (requestedPage == NULL) {
        enQueue(queue, hash, pageNumber);
        return 1; // In case of cache miss, it returns 1. We use a counter variable in the driver code to count the number of misses using this.

    // If required pageNumber is present in cache
    } else {

        // Unlink rquested pageNumber from its current location in queue.
        requestedPage->previous->next = requestedPage->next;
        if (requestedPage->next) {
            requestedPage->next->previous = requestedPage->previous;
        }

        // If the requested pageNumber is rear, then change rear as this QNode will be moved to front
        if (requestedPage == queue->rear) {
            queue->rear = requestedPage->previous;
            queue->rear->next = NULL;
        }

        // Put the requested pageNumber before current front
        requestedPage->next = queue->front;
        requestedPage->previous = NULL;

        requestedPage->next->previous = requestedPage;
        queue->front = requestedPage;

        // Increase the frequency of the referenced page
        requestedPage->frequency++;

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

    cout << "Enter your memory sequence in the order it has to be referenced: " << endl;

    Queue* cache = createQueue(cacheSize);

    for (int i = 0; i < memorySequenceLength; i++) {
        cin >> memorySequence[i];
    }

    Hash* hash = createHashMap(memorySequenceLength);

    for (int i = 0; i < memorySequenceLength; i++) {
        cacheMisses += referencePage(cache, hash, memorySequence[i]);
        /*if (referencePage(cache, hash, memorySequence[i]) == 1) {
            cout << memorySequence[i] << "is a miss!";
        }*/
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
