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
    linkedList->size = 0;

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
    lastAllocation = NULL;
	
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
    if (!node) {
        return NULL;
    }
    void* location = node->ptr;
    if (node->size == requested) {
        node->alloc = '1';
    } else {
        insert_before(node, requested, '1', node->ptr);
        node->ptr += requested;
        node->size -= requested;

        /*
        if (node->size == 0) {
            remove_node_with_zero_size(node);
        }
         */
    }

    return location;
}

void *allocBest(size_t requested) {
    Node* node = find_smallest_unallocated_node(requested);
    if (!node) {
        return NULL;
    }
    void* location = node->ptr;
    if (node->size == requested) {
        node->alloc = '1';
    }
    else {
        insert_before(node, requested, '1', node->ptr);
        node->ptr += requested;
        node->size -= requested;

        /*
        if (node->size == 0) {
            remove_node_with_zero_size(node);
        }
         */
    }

    return location;
}

void *allocWorst(size_t requested) {
    Node* node = find_largest_unallocated_node(requested);
    if (!node) {
        return NULL;
    }
    void* location = node->ptr;
    if (node->size == requested) {
        node->alloc = '1';
    }
    else {
        insert_before(node, requested, '1', node->ptr);
        node->ptr += requested;
        node->size -= requested;
    }

    return location;
}

void *allocNext(size_t requested) {
    Node* node = find_next_unallocated_node(requested);
    if (!node) {
        return NULL;
    }
    void* location = node->ptr;
    if (node->size == requested) {
        node->alloc = '1';
        if (!node->next)
            lastAllocation = linkedList->head;
        else
            lastAllocation = node;
    }
    else {
        insert_before(node, requested, '1', node->ptr);
        node->ptr += requested;
        node->size -= requested;
        lastAllocation = node;
    }

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
	            return allocBest(requested);
	  case Worst:
	            return allocWorst(requested);
	  case Next:
	            return allocNext(requested);
	  }
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block)
{
    remove_node_with_ptr(block);
    /*
    while (true) {
        Node* node = find_adjecent_unallocated_nodes();
        if (node == NULL) {
            return;
        } else {
            remove_node_with_ptr(node->ptr);
        }
    }
     */
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
        if ((node->alloc == '0') && (node->ptr < (myMemory + mySize)) && node->size != 0) {
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
        if (node->alloc == '0' && node->size <= size) {
            blocks_smaller_than_size++;
        }

        last = node;
        node = node->next;
    }

    return blocks_smaller_than_size;
}

/* Is a particular byte allocated or not? */

char mem_is_alloc(void *ptr)
{
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        if ((node->alloc == '1') && (node->ptr >= ptr && ptr <= node->ptr + node->size)) {
            return '1';
        }

        last = node;
        node = node->next;
    }

    return '0';
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


// aasdfa

int test_alloc_1(const char* strat_str) {
    strategies strategy;
    int lbound = 1;
    int ubound = 4;
    const char* strat = "first";

    if (strategyFromString(strat_str))
        lbound=ubound=strategyFromString(strat_str);

    for (strategy = lbound; strategy <= ubound; strategy++)
    {
        int correct_holes = 0;
        int correct_alloc = 100;
        int correct_largest_free = 0;
        int i;

        void* lastPointer = NULL;
        initmem(strategy,100);
        for (i = 0; i < 100; i++)
        {
            void* pointer = mymalloc(1);
            if ( i > 0 && pointer != (lastPointer+1) )
            {
                printf("Allocation with %s was not sequential at %i; expected %p, actual %p\n", strategy_name(strategy), i,lastPointer+1,pointer);
                return 1;
            }
            lastPointer = pointer;
        }

        if (mem_holes() != correct_holes)
        {
            printf("Holes not counted as %d with %s\n", correct_holes, strategy_name(strategy));
            return	1;
        }

        if (mem_allocated() != correct_alloc)
        {
            printf("Allocated memory not reported as %d with %s\n", correct_alloc, strategy_name(strategy));
            return	1;
        }

        if (mem_largest_free() != correct_largest_free)
        {
            printf("Largest memory block free not reported as %d with %s\n", correct_largest_free, strategy_name(strategy));
            return	1;
        }

    }

    return 0;
}

int test_alloc_2(const char* strat_str) {
    strategies strategy;
    int lbound = 1;
    int ubound = 4;

    if (strategyFromString(strat_str))
        lbound=ubound=strategyFromString(strat_str);

    for (strategy = lbound; strategy <= ubound; strategy++)
    {
        int correct_holes;
        int correct_alloc;
        int correct_largest_free;
        int correct_small;
        void* first;
        void* second;
        void* third;
        int correctThird;

        initmem(strategy,100);

        first = mymalloc(10);
        second = mymalloc(1);
        myfree(first);
        third = mymalloc(1);

        if (second != (first+10))
        {
            printf("Second allocation failed; allocated at incorrect offset with strategy %s", strategy_name(strategy));
            return 1;
        }

        correct_alloc = 2;
        correct_small = (strategy == First || strategy == Best);

        switch (strategy)
        {
            case Best:
                correctThird = (third == first);
                correct_holes = 2;
                correct_largest_free = 89;
                break;
            case Worst:
                correctThird = (third == second+1);
                correct_holes = 2;
                correct_largest_free = 88;
                break;
            case First:
                correctThird = (third == first);
                correct_holes = 2;
                correct_largest_free = 89;
                break;
            case Next:
                correctThird = (third == second+1);
                correct_holes = 2;
                correct_largest_free = 88;
                break;
            case NotSet:
                break;
        }

        if (!correctThird)
        {
            printf("Third allocation failed; allocated at incorrect offset with %s", strategy_name(strategy));
            return 1;
        }

        if (mem_holes() != correct_holes)
        {
            printf("Holes counted as %d, should be %d with %s\n", mem_holes(), correct_holes, strategy_name(strategy));
            return	1;
        }

        if (mem_small_free(9) != correct_small)
        {
            printf("Small holes counted as %d, should be %d with %s\n", mem_small_free(9), correct_small, strategy_name(strategy));
            return	1;
        }

        if (mem_allocated() != correct_alloc)
        {
            printf("Memory reported as %d, should be %d with %s\n", mem_allocated(0), correct_alloc, strategy_name(strategy));
            return	1;
        }

        if (mem_largest_free() != correct_largest_free)
        {
            printf("Largest memory block free reported as %d, should be %d with %s\n", mem_largest_free(), correct_largest_free, strategy_name(strategy));
            return	1;
        }

    }

    return 0;
}


int test_alloc_3(const char* strat_str) {
    strategies strategy;
    int lbound = 1;
    int ubound = 4;

    if (strategyFromString(strat_str))
        lbound=ubound=strategyFromString(strat_str);

    for (strategy = lbound; strategy <= ubound; strategy++)
    {
        int correct_holes = 50;
        int correct_alloc = 50;
        int correct_largest_free = 1;
        int i;

        void* lastPointer = NULL;
        initmem(strategy,100);
        for (i = 0; i < 100; i++)
        {
            void* pointer = mymalloc(1);
            if ( i > 0 && pointer != (lastPointer+1) )
            {
                printf("Allocation with %s was not sequential at %i; expected %p, actual %p\n", strategy_name(strategy), i,lastPointer+1,pointer);
                return 1;
            }
            lastPointer = pointer;
        }

        for (i = 1; i < 100; i+= 2)
        {
            myfree(mem_pool() + i);
        }

        if (mem_holes() != correct_holes)
        {
            printf("Holes not counted as %d with %s\n", correct_holes, strategy_name(strategy));
            return	1;
        }

        if (mem_allocated() != correct_alloc)
        {
            printf("Memory not reported as %d with %s\n", correct_alloc, strategy_name(strategy));
            return	1;
        }

        if (mem_largest_free() != correct_largest_free)
        {
            printf("Largest memory block free not reported as %d with %s\n", correct_largest_free, strategy_name(strategy));
            return	1;
        }

        for(i=0;i<100;i++) {
            if(mem_is_alloc(mem_pool()+i) == i%2) {
                printf("Byte %d in memory claims to ",i);
                if(i%2)
                    printf("not ");
                printf("be allocated.  It should ");
                if(!i%2)
                    printf("not ");
                printf("be allocated.\n");
                return 1;
            }
        }
    }

    return 0;
}


/* basic sequential allocation followed by 50 frees, then another 50 allocs */
int test_alloc_4(const char* strat_str) {
    strategies strategy;
    int lbound = 1;
    int ubound = 4;

    if (strategyFromString(strat_str))
        lbound=ubound=strategyFromString(strat_str);

    for (strategy = lbound; strategy <= ubound; strategy++)
    {
        int correct_holes = 0;
        int correct_alloc = 100;
        int correct_largest_free = 0;
        int i;

        void* lastPointer = NULL;
        initmem(strategy,100);
        for (i = 0; i < 100; i++)
        {
            void* pointer = mymalloc(1);
            if ( i > 0 && pointer != (lastPointer+1) )
            {
                printf("Allocation with %s was not sequential at %i; expected %p, actual %p\n", strategy_name(strategy), i,lastPointer+1,pointer);
                return 1;
            }
            lastPointer = pointer;
        }

        for (i = 1; i < 100; i+= 2)
        {
            myfree(mem_pool() + i);
        }

        for (i = 1; i < 100; i+=2)
        {
            void* pointer = mymalloc(1);
            if ( i > 1 && pointer != (lastPointer+2) )
            {
                printf("Second allocation with %s was not sequential at %i; expected %p, actual %p\n", strategy_name(strategy), i,lastPointer+1,pointer);
                return 1;
            }
            lastPointer = pointer;
        }

        if (mem_holes() != correct_holes)
        {
            printf("Holes not counted as %d with %s\n", correct_holes, strategy_name(strategy));
            return	1;
        }

        if (mem_allocated() != correct_alloc)
        {
            printf("Memory not reported as %d with %s\n", correct_alloc, strategy_name(strategy));
            return	1;
        }

        if (mem_largest_free() != correct_largest_free)
        {
            printf("Largest memory block free not reported as %d with %s\n", correct_largest_free, strategy_name(strategy));
            return	1;
        }

    }

    return 0;
}

/* performs a randomized test:
	totalSize == the total size of the memory pool, as passed to initmem2
		totalSize must be less than 10,000 * minBlockSize
	fillRatio == when the allocated memory is >= fillRatio * totalSize, a block is freed;
		otherwise, a new block is allocated.
		If a block cannot be allocated, this is tallied and a random block is freed immediately thereafter in the next iteration
	minBlockSize, maxBlockSize == size for allocated blocks is picked uniformly at random between these two numbers, inclusive
	*/
void do_randomized_test(int strategyToUse, int totalSize, float fillRatio, int minBlockSize, int maxBlockSize, int iterations)
{
    void * pointers[10000];
    int storedPointers = 0;
    int strategy;
    int lbound = 1;
    int ubound = 4;
    int smallBlockSize = maxBlockSize/10;

    if (strategyToUse>0)
        lbound=ubound=strategyToUse;

    FILE *log;
    log = fopen("tests.log","a");
    if(log == NULL) {
        perror("Can't append to log file.\n");
        return;
    }

    fprintf(log,"Running randomized tests: pool size == %d, fill ratio == %f, block size is from %d to %d, %d iterations\n",totalSize,fillRatio,minBlockSize,maxBlockSize,iterations);

    fclose(log);

    for (strategy = lbound; strategy <= ubound; strategy++)
    {
        double sum_largest_free = 0;
        double sum_hole_size = 0;
        double sum_allocated = 0;
        int failed_allocations = 0;
        double sum_small = 0;
        struct timespec execstart, execend;
        int force_free = 0;
        int i;
        storedPointers = 0;

        initmem(strategy,totalSize);

        clock_gettime(CLOCK_REALTIME, &execstart);

        for (i = 0; i < iterations; i++)
        {
            if ( (i % 10000)==0 )
                srand ( time(NULL) );
            if (!force_free && (mem_free() > (totalSize * (1-fillRatio))))
            {
                int newBlockSize = (rand()%(maxBlockSize-minBlockSize+1))+minBlockSize;
                /* allocate */
                void * pointer = mymalloc(newBlockSize);
                if (pointer != NULL)
                    pointers[storedPointers++] = pointer;
                else
                {
                    failed_allocations++;
                    force_free = 1;
                }
            }
            else
            {
                int chosen;
                void * pointer;

                /* free */
                force_free = 0;

                if (storedPointers == 0)
                    continue;

                chosen = rand() % storedPointers;
                pointer = pointers[chosen];
                pointers[chosen] = pointers[storedPointers-1];

                storedPointers--;

                myfree(pointer);
            }

            sum_largest_free += mem_largest_free();
            if (mem_holes() != 0)
                sum_hole_size += (mem_free() / mem_holes());
            sum_allocated += mem_allocated();
            sum_small += mem_small_free(smallBlockSize);
        }

        clock_gettime(CLOCK_REALTIME, &execend);

        log = fopen("tests.log","a");
        if(log == NULL) {
            perror("Can't append to log file.\n");
            return;
        }

        printf("\t=== %s ===\n",strategy_name(strategy));
        printf("\tTest took %.2fms.\n", (execend.tv_sec - execstart.tv_sec) * 1000 + (execend.tv_nsec - execstart.tv_nsec) / 1000000.0);
        printf("\tAverage hole size: %f\n",sum_hole_size/iterations);
        printf("\tAverage largest free block: %f\n",sum_largest_free/iterations);
        printf("\tAverage allocated bytes: %f\n",sum_allocated/iterations);
        printf("\tAverage number of small blocks: %f\n",sum_small/iterations);
        printf("\tFailed allocations: %d\n",failed_allocations);
        fclose(log);


    }
}

/* run randomized tests against the various strategies with various parameters */
int do_stress_tests(const char* strat)
{
    int strategy = strategyFromString(strat);

    unlink("tests.log");  // We want a new log file

    do_randomized_test(strategy,10000,0.25,1,1000,10000);
    do_randomized_test(strategy,10000,0.25,1,2000,10000);
    do_randomized_test(strategy,10000,0.25,1000,2000,10000);
    do_randomized_test(strategy,10000,0.25,1,3000,10000);
    do_randomized_test(strategy,10000,0.25,1,4000,10000);
    do_randomized_test(strategy,10000,0.25,1,5000,10000);

    do_randomized_test(strategy,10000,0.5,1,1000,10000);
    do_randomized_test(strategy,10000,0.5,1,2000,10000);
    do_randomized_test(strategy,10000,0.5,1000,2000,10000);
    do_randomized_test(strategy,10000,0.5,1,3000,10000);
    do_randomized_test(strategy,10000,0.5,1,4000,10000);
    do_randomized_test(strategy,10000,0.5,1,5000,10000);

    do_randomized_test(strategy,10000,0.5,1000,1000,10000); /* watch what happens with this test!...why? */

    do_randomized_test(strategy,10000,0.75,1,1000,10000);
    do_randomized_test(strategy,10000,0.75,500,1000,10000);
    do_randomized_test(strategy,10000,0.75,1,2000,10000);

    do_randomized_test(strategy,10000,0.9,1,500,10000);

    return 0; /* you nominally pass for surviving without segfaulting */
}


//asdf


int main() {
    test_alloc_1("first");
    test_alloc_2("first");
    test_alloc_3("first");
    test_alloc_4("first");

    test_alloc_1("best");
    test_alloc_2("best");
    test_alloc_3("best");
    test_alloc_4("best");

    test_alloc_1("worst");
    test_alloc_2("worst");
    test_alloc_3("worst");
    test_alloc_4("worst");

    test_alloc_1("next");
    test_alloc_2("next");
    test_alloc_3("next");
    test_alloc_4("next");

    do_stress_tests("first");
    do_stress_tests("best");
    do_stress_tests("worst");
    do_stress_tests("next");
}