# Re-implementing Malloc and Free
- Implemented __explicit free list__ structure
- Wrote mm_malloc to __find suitable memory block__, alter it to __fit explicit free list structure__, and __return a pointer to it__
- Wrote mm_free to __take in a memory block pointer__, alter the block to __fit the explicit free list structure__, __insert the block into the free list__, and __coalesce with surrounding blocks__
