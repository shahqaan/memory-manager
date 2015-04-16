/*
*
* This file contains definitions of all the functions used by memory_manager
*
*
*/



/*
 * Clear input buffer
 */
void buffer_clear();

/*
 * Initialize memory pool using malloc()
 * This memory pool is used by the memory manager to allocate memory to other modules
 * 
 * @param pool_size size of pool to be allocated in int
 */
void initialize_pool( int pool_size );


/*
 * Find the smallest possible free memory block greater than memory size required
 *
 * @param mem_size required size of memory block in int
 * @return pointer to the start address of the block found
 */
void* best_free_space( int mem_size );


/*
 * Allocate memory specified
 *
 * @param mem_size required size of memory block in int
 * @return pointer to the start address of the block allocated
 */
void *allocate_memory( int mem_size );


/*
 * Find fragments in memory pool and join them together wherever possible
 * Called twice everytime memory is free'ed
 * 
 * @param current address in the memory pool from where to start searching for memory fragments
 */
void defragment( void *current );


/* 
 * Free memory at the address specified
 * Calls defragment()
 *
 * @param current address of the block to be free'ed
 */
void free_memory( void *current );


/*
 * Increase size of already allocated memory
 * Check if memory block can be expanded,
 * if not then shift the block to new location and copy all of its contents
 *
 * @param current address of allocated memory
 * @param mem_size increment required in int
 * @return pointer to the start address of the block (same as @param current if relocation didn't take place)
 */
void* reallocate_memory( void *current, int mem_size );


/* This function displays the results of the test bed selected by the user */
void result();



/* resets the global counters in case user wants to test another test bed */
void reset();

