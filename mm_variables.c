/*
*
* This file contains all the global variables used by memory_manager
*
*
*/



void *start_address; /* contains starting address of memory pool */
void *last_address; /* contains last address of memory pool */

int pool_initialized = 0; /* 1 if the pool has been initialized */
int pool_memory; /* stores the size of the pool */

int total_alloc = 0; /*counter for total allocations */
int total_dealloc = 0; /*counter for total de-allocation */
int max_alloc = 0; /*stores maximum memory that has been allocated */
int min_alloc = 0; /*stores minimum memory that has been allocated */
float avg_alloc = 0; /*stores average size of allocation */
int total_fail = 0; /*stores number of failed allocation */   
time_t t1; /* To calculate time for allocations and de-allocations */
time_t t2; /* To calculate time for allocations and de-allocations */
time_t total_alloct = 0; /* Stores total time for allocations */ 
time_t total_dealloct = 0; /* Stores total time for de-allocations */ 
int realloc_mark = 0; /*If a memrory block is relocated to a new memory address then the space previously free'd won't be counted in deallocation, this is a marker for relocation */
int init_recordsize = 0; /* sets the first max_alloc and min_alloc values in allocate_memory */
int check_realloc = 0; /* This will check if the memory has been relocated to a new address in that case it wont increment number of allocations */ 

int debug = 1; /* Set to 0 if not debugging */
