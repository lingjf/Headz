#ifndef __SLABZ_H_e9dbe3ad_6934_4f68_8185_a52a076e52ee__
#define __SLABZ_H_e9dbe3ad_6934_4f68_8185_a52a076e52ee__

#ifdef __cplusplus
extern "C" {
#endif

/*objected memory allocation system*/

#define SLAB_RESERVED     0x1
#define SLAB_GREED        0x2
#define SLAB_GENEROUS     0x4

/********************************************************************************************
 * slab_initialize / slab_uninitialize - initialize/uninitialize slab system.
 *
 * Description: 
 *   slab_initialize() must and only must been called ONCE while lifetime.
 *   slab_uninitialize() should be called at the end. It will release all pending resource.
 *
 * INPUT : none 
 *
 * OUTPUT: none 
 *
 * RETURN: none
 */
void slab_initialize(void);
void slab_uninitialize(void);

/********************************************************************************************
 * slab_objector - Object constuctor function prototype.
 *
 * Description: 
 *   It is registered into slab when slab_create().
 *   It will be called when slab_kalloc().
 *
 * INPUT : 
 *   @1st parameter - slab handle
 *   @2nd parameter - the arg passed by slab_kalloc 2nd parameter.
 *
 * OUTPUT: none 
 *
 * RETURN: none
 */
typedef void ( slab_objector)(void*, void*);

/********************************************************************************************
 * slab_create - Constructs a object based allocation on the parameters passed in.
 *
 * Description: Google 'Think of slab or memcached' 
 *   slab_initialize() should has been called.
 *
 * INPUT : 
 *   @name - Readable name of slab, it will be copyed into internal of slab.
 *   @size - Size of object (bytes). range [1, ~]
 *   @mins - Minimum quantity of objects that reserved at create time, and hold all the time.
 *	  @maxs - Maximum quantity of objects. Request exceed this will get failure.
 *   @flags - Resource usage strategy.
 *       *SLAB_RESERVED  -  Maximum objects were reserved at create time, and hold all the time.
 *       *SLAB_GREED     -  Free memory pages were not released initiatively.
 *       *SLAB_GENEROUS  -  Free memory pages were release initiatively.
 *       *SLAB Default   -  Release free memory exceed 50% percentage.
 *   @growth_factor - Memory page growth factor strategy.
 *       *   > 1  - Add such number of objects every time. If less 8K, add 8K
 *       * (0, 1] - Percentage of already allocated resource. If less 8K, add 8K
 *       *   = 0  - Default strategy. 80% already allocated. If less 8K, add 8K
 *   @ctor - Constructor of objects. NULL is allowed.
 *
 * OUTPUT: 
 *   @return - constructed slab handle
 *
 * RETURN: NOT NULL if successful otherwise NULL.
 */
void* slab_create(char *name, unsigned long size, unsigned long mins, unsigned long maxs, unsigned long flags,
                  float growth_factor, slab_objector *ctor);

/********************************************************************************************
 * slab_destroy - Destructs a object based allocation.
 *
 * Description: 
 *   slab_initialize() should has been called.
 *   slab_create() should has been called.
 *
 * INPUT : 
 *   @handle - The slab handle return from slab_create().
 *
 * OUTPUT: none 
 *
 * RETURN: none
 */
void slab_destroy(void *handle);

/********************************************************************************************
 * slab_shrink - Force recycle the free memory pages.
 *
 * Description: You Know 
 *   slab_initialize() should has been called.
 *
 * INPUT : 
 *   @handle - The slab handle return from slab_create().
 *      *NULL     - recycle free pages from all slabs.
 *      *NOT NULL - recycle free pages from this slab.
 *
 * OUTPUT: none 
 *
 * RETURN: none
 */
void slab_shrink(void *handle);

/********************************************************************************************
 * slab_kalloc / slab_kfree - allocate a object / free a object.
 *
 * Description: You Know 
 *   slab_initialize() should has been called.
 *   slab_create() should has been called.
 *   size of object is specified in slab_create().
 *
 * INPUT : 
 *   @handle - The slab handle return from slab_create().
 *   @arg    - It will pass to slab_ctor if slab_ctor exist.
 *   @ptr    - The pointer of object.
 *
 * OUTPUT: return - object pointer, none for kfree 
 *
 * RETURN: object pointer if successful otherwise NULL, none for kfree.
 */
void* slab_kalloc(void *handle, void *arg);
void slab_kfree(void *handle, void *ptr);

/********************************************************************************************
 * slab_malloc / slab_mfree - allocate / free a piece of memory.
 *
 * Description: You Know 
 *   slab_initialize() should has been called.
 *   It allocate memory from builtin slab
 *
 * INPUT : 
 *   @size   - Size of piece of memory.
 *   @ptr    - The pointer of memory.
 *
 * OUTPUT: return - memory pointer, none for mfree 
 *
 * RETURN: memory pointer if successful otherwise NULL, none for mfree.
 */
void* slab_malloc(unsigned long size);
void slab_mfree(void *ptr);

#define SLAB_OK                    0
#define SLAB_OUT_OF_MEMORY_EC     -1
#define SLAB_OUT_OF_LIMITATION_EC -2
#define SLAB_BUSY_EC              -3
#define SLAB_CELL_MAGIC_EC        -4

int __slab_inside_cell_size(void *handle);
int __slab_inside_flags(void *handle);
int __slab_inside_object_head_size(void *handle);
int __slab_inside_cell_head_size(void *handle);
int __slab_inside_cell_tail_size(void *handle);
int __slab_inside_page_head_size(void *handle);
int __slab_inside_slab_head_size(void *handle);
int __slab_inside_total_objs(void *handle);
int __slab_inside_free_objs(void *handle);
int __slab_inside_free_pages(void *handle);
int __slab_inside_busy_pages(void *handle);
int __slab_inside_full_pages(void *handle);
int __slab_inside_pages(void *handle);

#ifdef __cplusplus
}
#endif

#endif

