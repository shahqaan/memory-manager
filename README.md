# memory-manager


This file provides user with an API for Pool initialization. It initializes memory pool according to the size provided to the program. Best free space implements the best fit algorithm to find block with optimum size. Allocate_memory, allocates memory to different modules with a structure at the begining of every block. Defragment is implemented to remove the complications when too many small blocks starts to exist with in the memory making it impossible for large process to run. Free memory, frees a block allocated to a module and makes it available for any other module. If a module who has already been allocated memory wish to use more memory in that case reallocate memory allocates more memory to that block or either re-locate it to a new place.




