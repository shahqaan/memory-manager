/*
*
* This file contains structure(s) used by memory_manager
*
*
*/




/* 
* memory control block: contains information regarding a particular memory block 
* In the memory pool, every block of memory has a structure at the begining 
* of 8 byte size. this structure holds information of the respective memory block
* such as memory block size and whether its currently in use.  
*/

struct memory_control_block 
{
  int is_used; /* if the memory block is being used its value is set to 1, otherwise 0 */
  int block_size; /* contains size of the memory block */
};
