#ifndef SLAB_ALLOCATOR_H
#define SLAB_ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct slab slab_t;
typedef struct slab_cache slab_cache_t;

/**
 * Creates a new slab cache for objects of a given size.
 * @param obj_size Size of each object (in bytes).
 * @param objs_per_slab Number of objects per slab.
 * @return Pointer to the created slab cache, or NULL on failure.
 */
slab_cache_t* slab_cache_create(size_t obj_size, size_t objs_per_slab);

/**
 * Destroys the slab cache and all associated slabs.
 * @param cache Pointer to the slab cache.
 */
void slab_cache_destroy(slab_cache_t* cache);

/**
 * Allocates an object from the slab cache.
 * @param cache Pointer to the slab cache.
 * @return Pointer to the allocated object, or NULL if allocation fails.
 */
void* slab_alloc(slab_cache_t* cache);

/**
 * Frees an object back to the slab cache.
 * @param cache Pointer to the slab cache.
 * @param obj Pointer to the object to free.
 */
void slab_free(slab_cache_t* cache, void* obj);

#ifdef __cplusplus
}
#endif

#endif // SLAB_ALLOCATOR_H