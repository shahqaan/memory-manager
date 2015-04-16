#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "src/mm_variables.c"
#include "src/mm_functions.c"
#include "src/mm_structures.c"



void buffer_clear()
{

  int free_buffer;
  while((free_buffer=fgetc(stdin)) != EOF && free_buffer!='\n');

}

void initialize_pool( int pool_size ) 
{
  /* 
   * Initialize memory and hold its start and last address
   */
  start_address = malloc( pool_size ); /*  allocates memory and returns pointer to start_address*/
  last_address = start_address + pool_size; /* Stores last address of the memory */

 


  pool_initialized = 1; /* pool has been initialized */
  pool_memory = pool_size;

  void *initialize_temp = start_address;
  while ( initialize_temp <= last_address ) {
    struct memory_control_block* temp_mcb = (struct memory_control_block*)initialize_temp;
    temp_mcb->is_used = 0;
    initialize_temp++;
  }

  struct memory_control_block *temp = (struct memory_control_block*)start_address; /* Points the structure pointer towards start_address */
 

  /* Notify about initialization of memory pool */
  if ( debug )
    {
      printf( "start_address: %d\n", (int) start_address );    
      printf( "last_address %d\n", (int) last_address );       
    }

  /*
   * Make a block of free memory
   * Keep its data in a structure residing at the start of the block (and hence the pool)
   */
  temp->is_used = 0; /* Current memory block is not being used */
  temp->block_size = pool_size - sizeof( struct memory_control_block ); /* current block takes whole place of pool minus size of the structure */

  /* notify about initialization of memory block */
  printf("Memory pool has been initialized\n");
  if ( debug )
    printf( "block size: %d\n", temp->block_size );

}

void* best_free_space( int mem_size )
{

  void *current = start_address; /* point current towards starting address */
  void *final_location = current;
  struct memory_control_block *current_memory_control_block;
  void *temp = 0;
  while ( current < last_address )
    {
      current_memory_control_block = ( struct memory_control_block* ) current; 
      if ( !current_memory_control_block->is_used ) /* if memory control block is not being used */
        {
          if ( current_memory_control_block->block_size >= mem_size ) /* if block size available is greater than size requested */
            {
	if ( temp != 0 ) /*temp should be pointing to a free block to compare with current */
	  {
	    struct memory_control_block *temp_control_block = ( struct memory_control_block* ) temp;
	    //  printf( "previous block size: %d\n", temp_control_block->block_size );  for debug purpose
	    //  printf( "current block size: %d\n", current_memory_control_block->block_size ); for debug purpose
	    if ( current_memory_control_block->block_size < temp_control_block->block_size ) /* if this block is smaller than previous then save this address */
	      {
	        final_location = current; /* update the final_location */
	      }
	  }
	temp = current; /* save the address of free block */
            }
        }

      current = current + sizeof( struct memory_control_block ) + current_memory_control_block->block_size; /* jump to next location */

    }
  return final_location;

}

void *allocate_memory( int mem_size )
{
  time(&t1); /* Initializing time to calulate total time for allocation */
  void *memory_location = 0; /*location of memory allocated. 0 means that memory has not been allocated */
  void *current = best_free_space( mem_size ) ; /* Finds the address of best free space */

  struct memory_control_block *current_memory_control_block = (struct memory_control_block*) current; /* Points current_memory_control_block towards current location */

  int remaining_block_size = -1; /* holds the original size of the block. -1 means that no block size has been saved yet */
  int allocated = 0; /* checks if the memory has been allocated. 0 means that memory has not been allocated */
  int overlap =0; /* checks if the structures overlap or are adjacent */

 

  /********************** First-Fit Algorithm ************************************/

  while ( current < last_address ) /* check if current stays within the memory pool */

    {
      if ( !(current_memory_control_block->is_used) ) /* if current_memory_control_block is not used */
	{

	  if ( current_memory_control_block->block_size >=  mem_size ) /* if block_size available is greater than the size requested */
	    { 


	      
/*When memory allocated, a new control block is created at the end of it to store remaining size and the availablilty of the newly created block.
It might happen that the newly created control block may over lap with an already existing control e.g if size of a block is 500 bytes and we alllocate exactly 500 bytes in that case the structure created after allocating 500 bytes would override an already existing structure. If we allocate 492 bytes in that case the new structure would have size 0 and would be next to an other structure already existing in the memory which is also a problem. Any size between 492 and 500 would cause memory overide to avoid this situation instead of creating structure after allocating memory we allocate the whole memory block. Note: In worst case max 8 extra bytes will allocated which isn't so bad    */

	      if((int)(current_memory_control_block->block_size-mem_size-(sizeof(struct memory_control_block)))  <= 0)
		{

		
		  overlap = 1; /* memory overlap condidtion has been marked */

		  /* we increase the memory requested to whole block size other wise ther will be overlapping of structures(memory override)*/ 
                  mem_size = current_memory_control_block->block_size; 

		} 
              
              /* we can allocate the memory now */

	      current_memory_control_block->is_used = 1; /* memory block is being used now */
	      int prev_block_size = current_memory_control_block->block_size; /* save original size of the free block */
	      current_memory_control_block->block_size = mem_size; /* size of the block is equal to mem_size */
	      allocated = 1; /* memory allocated */
              printf("Memory has been succesfully allocated\n");  
              if(check_realloc != 1)  /* This is in case a memory is relocated to new address. We won't increment the number of allocations and average allocation */
		{
		  total_alloc++; /* counts total number of allocations */
		  avg_alloc = avg_alloc + mem_size; /* calculates total memory that has been allocated yet */
		}
              
              
              /* This sets the values of max_alloc and min_alloc */
              if( init_recordsize == 0) /* checks if this is the very first allocation. 0 means its true */
		{
		  max_alloc = mem_size;  /* Set the maximum block size */
		  min_alloc = mem_size;  /* Set the minimum block size */
		  init_recordsize=1;     /* Set to 1 so this condition wont ever be true again */  
		}

              if( init_recordsize == 1 ) /* Checks if this isn't the first time memory is beng allocated */
		{
		  if(mem_size > max_alloc ) /* if the newly allocated block size is greater than previous max_alloc */
		    {
		      max_alloc = mem_size;
		    }
		  if(mem_size < min_alloc ) /* if the newly allocated size is smaller than previous min_alloc */
		    {
		      min_alloc = mem_size;
		    }
		}

 
	      remaining_block_size = prev_block_size - current_memory_control_block->block_size - sizeof( struct memory_control_block ); /* remaining unused space */
	      /* MEMORY HAS BEEN ALLOCATED! */
	      memory_location = current;
               
	      /* Below printf's are used for debugging */
	      /* notify about memory allocation */
	      //  printf( "\nstart address of this memory location: %x\n", current );
	      //  printf( "start address of memory control block: %x\n", current_memory_control_block );
	      //  printf( "block size: %d\n", current_memory_control_block->block_size );
	      
	      /* information about block remaining block sizes */
	      // printf( "\nRemaining block!\n" );
	      // printf( "prev block size: %d\n", prev_block_size );
	      // printf( "current block size: %d\n", current_memory_control_block->block_size );
	      // printf( "remaining block size: %d\n", remaining_block_size );

	      break;



	    }
	}
      current = current +  (sizeof( struct memory_control_block )) +  current_memory_control_block->block_size; /* assuming that at every instance, there is a memory block */
      current_memory_control_block = (struct memory_control_block*) current; /* move the control block pointer to next location */

    }



  /* if memory has been allocated we create a new structure at the end of recently allocated block with its availabilty and size updated with no memory overlap*/
  if ( allocated && !overlap ) 
    {

      /* now search for the next free space and add its data to the control block */
      void *temp_location = current + sizeof( struct memory_control_block ) + current_memory_control_block->block_size; /* move the temp pointer to the next control block  */
      current_memory_control_block = (struct memory_control_block*) temp_location; /* point the current memory block towards new location */

      while ( temp_location < last_address ) /* to ensure that current stays within the memory pool */
	{
	  if ( !( current_memory_control_block->is_used) ) /* if current memory block is not used */
	    {
	      current_memory_control_block->block_size = remaining_block_size; /* give the size of remaining unused memory to this mcb */

	      /* notify about free space allocated */
	      //   printf( "\nstart address of free block: %x\n", temp_location ); 
	      //   printf( "start address of memory control block: %x\n", current_memory_control_block );
	      //   printf( "block size: %d\n", current_memory_control_block->block_size );
	      //   printf( "\nThis memory block has been successfully allocated!\n" );
	      break;
	    }

	

	}
    }

  if ( !memory_location ) /* if memory location doesn't exist */
    {
      total_fail++; /*counts total number of allocations failed */
      printf( "\nNot enough space in memory pool!" );
    }


  time(&t2); /*  Time Stopped so we may calulate total time for allocation */
  total_alloct = total_alloct + difftime( t2,t1 );
  
  return memory_location;

}

void defragment( void *current )
{


  struct memory_control_block *current_memory_control_block;
  while ( current < last_address ) /* while current is inside memory pool */
    { 
      current_memory_control_block = ( struct memory_control_block* ) current; /* point control block to current location */
 
    
      if ( current_memory_control_block->is_used == 0 ) /* if current memory block is not being used */

        {
          void *temp = current; /* point temp towards current free block location */


          temp = temp + sizeof( struct memory_control_block ) + current_memory_control_block->block_size; /* move temp to next block location */

          struct memory_control_block *temp_memory_control_block;

          temp_memory_control_block = ( struct memory_control_block* ) temp; /* point temp control block to temp location */


          if( temp_memory_control_block->is_used == 0 && temp < last_address ) /* if temp is also available and is inside memory pool */

            {
              /* merges the blocks */
	current_memory_control_block->block_size = current_memory_control_block->block_size + sizeof( struct memory_control_block ) + temp_memory_control_block->block_size; 

	// printf("defragmentation successful %d\n", current_memory_control_block->block_size); for debug purpose

            }

          else   
            { 

	current = current + sizeof( struct memory_control_block ) + current_memory_control_block->block_size;
            }
        }

      else
        {
          current = current + sizeof( struct memory_control_block ) + current_memory_control_block->block_size;
        }

    }
}





void free_memory( void *current ) 
{
  time(&t1); /* Initializing time to calulate total time for De-allocation */

  if ( current >= start_address && current <= last_address ) /* location lies within memory pool */
    {
      /* cast the pointer to current location to the control block structure type and save its location */
      struct memory_control_block *current_memory_control_block = ( struct memory_control_block* ) current; 

      if ( current_memory_control_block->is_used == 1 ) /* if the memory block is being used */
        {
          current_memory_control_block->is_used = 0; /* memory freed */

          time(&t2); /*  Time Stopped so we may calulate total time for allocation */
          total_dealloct = total_dealloct + difftime( t2,t1 );
       
          if(realloc_mark == 0) /*if the memory is relocated we wont count it as de-allocation */ 
            {
	total_dealloc++; /* counts total number of de-allocations */ 
	printf("Memory deallocated\n"); 
            }
	 
          /* look for free memory fragments and defragment them  */
          defragment( start_address );

          //  printf( "\nMemory freed at location: %x\n", current_memory_control_block ); /* notify */  for debug purpose
        }
      else /* if the memory block is not being used */
        {
          printf( "\nMemory doesn't exist!\n" ); /* notify */
        }
    }
  else /* location doesn't lie between memory pool */
    {
      printf( "\nMemory location can't be freed because it doesn't lie between memory pool\n" ); /* notify */
    }


}

void* reallocate_memory( void *current, int mem_size )
{

  int reallocated = 0; /* set to be 1 if memory has been reallocated */
  /*
   * final location is initially set to location provided
   * change final_location to new location of memory if we need to move the block
   */
  void *final_location = current;
  struct memory_control_block *current_memory_control_block = ( struct memory_control_block* ) current;


  if ( current >= start_address && current <= last_address ) /* current lies within memory pool */
    {
      /* check if the block in front of current block is free 
       * We will be compromising best fit in case if next block is free
       * Since we wont have to relocate the data as its expensive procedure
       */
      void *temp = current + sizeof( struct memory_control_block ) + current_memory_control_block->block_size; /* point temp to next location */
      struct memory_control_block *temp_control_block = ( struct memory_control_block* ) temp; /* point the temp_control_block to current location */
      if ( !temp_control_block->is_used ) /* check if next block is free */
        {
          if ( temp_control_block->block_size >= mem_size ) /* if next block is larger than requested size */
            {

	/*When memory allocated, a new control block is created at the end of it to store remaining size and the availablilty of the newly created block.
	  It might happen that the newly created control block may over lap with an already existing control e.g if size of a block is 500 bytes and we alllocate exactly 500 bytes in that case the structure created after allocating 500 bytes would override an already existing structure. If we allocate 492 bytes in that case the new structure would have size 0 and would be next to an other structure already existing in the memory which is also a problem. Any size between 492 and 500 would cause memory overide to avoid this situation instead of creating structure after allocating memory we allocate the whole memory block. Note: In worst case max 8 extra bytes will allocated which isn't so bad    */

	if((int)(temp_control_block->block_size - mem_size) <= 0 )
	  {
	    //  printf(" overlapping in reallocation ");  for debug purpose

	    /* to avoid overlapping of the structure we will alocate the entire block memory to the previous block Note:in worst case only 8 extra bytes will be alocated to the new block */ 	    
	    current_memory_control_block->block_size = current_memory_control_block->block_size + temp_control_block->block_size; /* merges the two blocks */
	    //  printf("new size of the block is :%d\n",current_memory_control_block->block_size);  for debug purpose
		 
	    avg_alloc = avg_alloc + temp_control_block->block_size; /* avg_alloc already has the previous size so we just need to include the realloc size */

	    if( init_recordsize == 1 ) /*if the memory is reallocated than we must check for new maximum block size */
	      {
	        if(current_memory_control_block->block_size > max_alloc ) /* if the new memory block size is greater than maximun block size allocated so far */ 
	          {
	            max_alloc = current_memory_control_block->block_size;
	          }
	      }

	    printf( "\nMemory reallocated!\n" ); 
	    reallocated = 1;

	  }
	else  /* in case ther isn't any overlapping */ 
	  {
		
	    int remaining_block_size = temp_control_block->block_size - mem_size ; /* calculate the remaining block size */
	    current_memory_control_block->block_size = current_memory_control_block->block_size + mem_size; /* increase the size of the block */
	    avg_alloc = avg_alloc + mem_size; /* avg_alloc already has the previous size so we just need to include the realloc size */
              
	    if( init_recordsize == 1 ) /*if the memory is reallocated than we must check for new maximum block size */
	      {
	        if(current_memory_control_block->block_size > max_alloc ) /* if the new memory block size is greater than maximun block size allocated so far */ 
	          {
	            max_alloc = current_memory_control_block->block_size;
	          }
	      }
	    temp = temp + mem_size; /* move temp to end of memory location */
	    temp_control_block = ( struct memory_control_block* ) temp;
	    temp_control_block->is_used = 0;
	    temp_control_block->block_size = remaining_block_size;

	    printf( "\nMemory reallocated!\n" );
	    reallocated = 1;

	  }
            }
        }
      
      /*
       * since next block is either allocated or doesn't have enough space
       * we'll start searching for a free block to move our original block to that place
       */



      if ( reallocated == 0 )
        {
	  

          check_realloc = 1; /* when a new memory block will be allocated this check woudn't increment number of allocations */
          int new_mem_size = current_memory_control_block->block_size + mem_size; /* total memory size */
          void *original_location = current; /* save the original memory block location */
          current = start_address; /* point current to start of pool */
          current_memory_control_block = ( struct memory_control_block* ) current; 
          current = allocate_memory( new_mem_size ); /* allocate total memory */
          check_realloc = 0; /* marker reset */
          if ( current != 0 ) /* if successfully allocated */
            {
	final_location = current; /* memory has been allocated. Update final_location */
	struct memory_control_block *temp_control_block = ( struct memory_control_block* ) original_location;
	char *temp_old  = (char*)original_location + sizeof( struct memory_control_block ); /* address where the old data resides */
	char *temp_new = (char*)final_location + sizeof(struct memory_control_block ); /* address where new data should go */
              avg_alloc = avg_alloc + mem_size; /* calculates total memory that has been allocated yet */
	for ( ; ; ) /* copy data to new space */
	  {
	    *temp_new = *temp_old; /* copy current byte to new location */
	    temp_new = temp_new + sizeof( *temp_new ); /* move temp_new to next location */
	    temp_old = temp_old + sizeof( *temp_old ); /* move temp_old to next location */
	    if ( ( (void*)temp_old - original_location - sizeof( struct memory_control_block ) ) >= temp_control_block->block_size )
	      {
	        printf( "\nData successfully copied to new address\n" ); 
                      realloc_mark=1; /*Not to count deallocation in free memory function */
	        free_memory( original_location ); /* free original block of data */
	        realloc_mark=0; /*setting value to again 0 */ 
                      break;
	      }

	  }

            }
        }



    }
  else /* current doesn't lie between memory pool */
    {
      printf( "\nThe location you provided doesn't lie within memory pool!\n" );
    }
  struct memory_control_block *temp_1 = ( struct memory_control_block* ) final_location;
  //  printf( "new block size: %d\n", temp_1->block_size ); for debug purpose
  return final_location;

}


void result()
{
  printf("\n**************************************\n");
  printf("The size of the memory pool : %dMB\n", pool_memory );
  printf("\n**************************************\n");
  printf("Total number of Allocations : %d\n",total_alloc);
  printf("\n**************************************\n");
  printf("Total number of De-allocations : %d\n",total_dealloc);
  printf("\n**************************************\n");
  printf("Total Number of allocations failed : %d\n",total_fail);
  printf("\n**************************************\n");
  printf("Size of maximun allocation : %dBytes\n",max_alloc);
  printf("\n**************************************\n");
  printf("Size of minimum allocation : %dBytes\n",min_alloc);
  printf("\n**************************************\n");
  printf("Size of average allocation : %fBytes\n",avg_alloc/total_alloc);
  printf("\n**************************************\n");
  printf("Total allocation time : %fseconds\n",(float)total_alloct);
  printf("\n**************************************\n");
  printf("Total deallocation time : %fseconds\n",(float)total_dealloct);
  printf("\n**************************************\n");

}


void reset()
{
  pool_initialized = 0; /* 1 if the pool has been initialized */
  total_alloc = 0; /*counter for total allocations */
  total_dealloc = 0; /*counter for total de-allocation */
  max_alloc = 0; /*stores maximum memory that has been allocated */
  min_alloc = 0; /*stores minimum memory that has been allocated */
  avg_alloc = 0; /*stores average size of allocation */
  total_fail = 0; /*stores number of failed allocation */   
  time_t total_alloct = 0; /* Stores total time for allocations */ 
  time_t total_dealloct = 0; /* Stores total time for de-allocations */ 
  realloc_mark = 0; /*If a memrory block is relocated to a new memory address then the space previously free'd won't be counted in deallocation, this is a marker for relocation */
  init_recordsize = 0; /* sets the first max_alloc and min_alloc values in allocate_memory */
  check_realloc = 0; /* This will check if the memory has been relocated to a new address in that case it wont increment number of allocations */ 
}

