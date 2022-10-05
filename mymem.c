#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>

#include "linked_list.h"

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memBlock
{
  // doubly-linked list
  struct memBlock *last;
  struct memBlock *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct memBlock *head;
static struct memBlock *next;


void create_list(size_t capacity) {
    linkedList = malloc(sizeof(LinkedList)) ;
    linkedList -> head = NULL ;
    linkedList -> tail = NULL ;

    insert_first(capacity, '0', myMemory);
}

struct memBlock* createMemblock(struct memBlock* last, struct memBlock* next, int size, char alloc) {
    struct memBlock* block = malloc(sizeof(struct memBlock));
    block->last = last;
    block->next = next;
    block->size = size;
    block->alloc = alloc;
    block->ptr = last->ptr + last->size;
}


/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz)
{
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL)
        free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */
    free(head);
    free(linkedList);
	
	/* TODO: Initialize memory management structure. */
    myMemory = malloc(sz);
    create_list(mySize);
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

// First-fit: select the first suitable block with smallest address.
void *allocFirst(size_t requested)
{
    Node* node = find_first_node_with_capacity(requested);
    void* location = node->ptr;
    insert_before(node, requested, '1', node->ptr);
    node->ptr += requested;
    node->size -= requested;

    return location;
}

void *mymalloc(size_t requested)
{
	assert((int)myStrategy > 0);

	switch (myStrategy)
	  {
	  case NotSet: 
	            return NULL;
	  case First:
	            return allocFirst(requested);
	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    remove_node_with_ptr(block);
    while (true) {
        Node* node = find_adjecent_unallocated_nodes();
        if (node == NULL) {
            return;
        } else {
            remove_node_with_ptr(node->ptr);
        }
    }
}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes()
{
    int holes = 0;
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        if (node->alloc == '0' && node->ptr < myMemory + mySize) {
            ++holes;
        }

        last = node;
        node = node->next;
    }
    return holes;
}

/* Get the number of bytes allocated */
int mem_allocated()
{
    int allocated = 0;
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        if (node->alloc == '1') {
            allocated += node->size;
        }

        last = node;
        node = node->next;
    }

    return allocated;
}

/* Number of non-allocated bytes */
int mem_free()
{
	return mem_total() - mem_allocated();
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free()
{
    int largest_unallocated = 0;
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        if (node->alloc == '0') {
            if (node->size > largest_unallocated) {
                largest_unallocated = node->size;
            }
        }

        last = node;
        node = node->next;
    }
    return largest_unallocated;
}

/* Number of free blocks smaller OR EQUAL TO than "size" bytes. */
int mem_small_free(int size)
{
    int blocks_smaller_than_size = 0;
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        if (node->alloc == "0" && node->size <= size) {
            blocks_smaller_than_size++;
        }

        last = node;
        node = node->next;
    }
}

/* Is a particular byte allocated or not? */

char mem_is_alloc(void *ptr)
{
    return 0;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory()
{
    Node* last;
    Node* node = linkedList->head;
    printf("[");
    while (node != NULL) {
        printf("(size: %i, allocated: %c, location: %i), ", node->size, node->alloc, node->ptr);
        last = node;
        node = node->next;
    }
    printf("]\n\n");
}

void print_short() {
    Node* last;
    Node* node = linkedList->head;
    printf("[");
    while (node != NULL) {
        printf("(%i, %c, %i), ", node->size, node->alloc, node->ptr - myMemory);
        last = node;
        node = node->next;
    }
    printf("]\n\n");
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);
	
	print_memory();
	print_memory_status();
}


/*
int main() {
    initmem(strategyFromString("first"), 500);
    print_short();
    void* a = mymalloc(100);
    print_short();
    void* b = mymalloc(100);
    print_short();
    void* c = mymalloc(100);
    print_short();
    myfree(b);
    print_short();
    myfree(c);
    print_short();
}
 */
