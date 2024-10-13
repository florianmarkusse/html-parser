#include "flo/util/memory/buddy-allocator.h"
#include "flo/util/assert.h"

flo_BuddyBlock *flo_splitBuddy(flo_BuddyBlock *block, size_t size) {
    FLO_ASSERT(size > 0);

    if (block != NULL) {
        while (size * 2 < block->size) {
            size_t halfSize = block->size >> 1;
            block->size = halfSize;
            block = flo_nextBuddy(block);
            block->size = halfSize;
            block->isFree = true;
        }

        if (size <= block->size) {
            return block;
        }
    }

    return NULL;
}

flo_BuddyBlock *flo_findBestBuddy(flo_BuddyBlock *head, flo_BuddyBlock *tail,
                                  size_t size) {
    FLO_ASSERT(size > 0);

    flo_BuddyBlock *bestBlock = NULL;
    flo_BuddyBlock *block = head;                 // Left Buddy
    flo_BuddyBlock *buddy = flo_nextBuddy(block); // Right Buddy

    // The entire memory section between head and tail is free,
    // just call 'buddy_block_split' to get the allocation
    if (buddy == tail && block->isFree) {
        return flo_splitBuddy(block, size);
    }

    // Find the block which is the 'bestBlock' to requested allocation sized
    while (block < tail &&
           buddy < tail) { // make sure the buddies are within the range
        // If both buddies are free, coalesce them together
        // NOTE: this is an optimization to reduce fragmentation
        //       this could be completely ignored
        if (block->isFree && buddy->isFree && block->size == buddy->size) {
            block->size <<= 1;
            if (size <= block->size &&
                (bestBlock == NULL || block->size <= bestBlock->size)) {
                bestBlock = block;
            }

            block = flo_nextBuddy(buddy);
            if (block < tail) {
                // Delay the buddy block for the next iteration
                buddy = flo_nextBuddy(block);
            }
            continue;
        }

        if (block->isFree && size <= block->size &&
            (bestBlock == NULL || block->size < bestBlock->size)) {
            bestBlock = block;
        }

        if (buddy->isFree && size <= buddy->size &&
            (bestBlock == NULL || buddy->size < bestBlock->size)) {
            // If each buddy are the same size, then it makes more sense
            // to pick the buddy as it "bounces around" less
            bestBlock = buddy;
        }

        if (block->size < buddy->size) {
            block = flo_nextBuddy(buddy);
            if (block < tail) {
                // Delay the buddy block for the next iteration
                buddy = flo_nextBuddy(block);
            }
        } else {
            // Buddy was split into smaller blocks
            block = buddy;
            buddy = flo_nextBuddy(buddy);
        }
    }

    if (bestBlock != NULL) {
        // This will handle the case if the 'best_block' is also the perfect fit
        return flo_splitBuddy(bestBlock, size);
    }

    // Maybe out of memory
    return NULL;
}

flo_BuddyAllocator flo_createBuddyAllocator(char *data, size_t size) {
    FLO_ASSERT(size > 0);
    FLO_ASSERT((size & (size - 1)) == 0);
    FLO_ASSERT(size > (size_t)sizeof(flo_BuddyBlock));

    flo_BuddyAllocator result;

    result.head = (flo_BuddyBlock *)data;
    result.head->size = size;
    result.head->isFree = true;

    result.tail = flo_nextBuddy(result.head);

    return result;
}

/**
 * Coalescing algorithm. Think of left and right of 2 nodes of a binary tree.
 * You can only merge nodes that are actually part of the same branch.
 */
void flo_coalesceBuddies(flo_BuddyBlock *head, flo_BuddyBlock *tail) {
    while (true) {
        // Keep looping until there are no more buddies to coalesce

        flo_BuddyBlock *block = head;
        flo_BuddyBlock *buddy = flo_nextBuddy(block);

        bool noCoalesce = true;
        while (block < tail &&
               buddy < tail) { // make sure the buddies are within the range
            if (block->size < buddy->size) {
                block = flo_nextBuddy(buddy);
                if (block < tail) {
                    // Leave the buddy block for the next iteration
                    buddy = flo_nextBuddy(block);
                }
            } else if (block->size > buddy->size) {
                // The buddy block is split into smaller blocks
                block = buddy;
                buddy = flo_nextBuddy(buddy);
            } else {
                if (block->isFree && buddy->isFree) {
                    // Coalesce buddies into one
                    block->size <<= 1;
                    block = flo_nextBuddy(block);
                    if (block < tail) {
                        buddy = flo_nextBuddy(block);
                        noCoalesce = false;
                    }
                } else {
                    block = flo_nextBuddy(buddy);
                    if (block < tail) {
                        // Leave the buddy block for the next iteration
                        buddy = flo_nextBuddy(block);
                    }
                }
            }
        }

        if (noCoalesce) {
            return;
        }
    }
}

__attribute((unused, malloc, alloc_size(2, 3))) void *
flo_buddyAlloc(flo_BuddyAllocator *buddyAllocator, ptrdiff_t size,
               ptrdiff_t count, unsigned char flags) {
    FLO_ASSERT(size > 0);

    size_t total = size * count;
    size_t totalSize = total + FLO_SIZEOF(flo_BuddyBlock);

    flo_BuddyBlock *found = flo_findBestBuddy(buddyAllocator->head,
                                              buddyAllocator->tail, totalSize);
    if (found == NULL) {
        // Try to coalesce all the free buddy blocks and then search again
        flo_coalesceBuddies(buddyAllocator->head, buddyAllocator->tail);
        found = flo_findBestBuddy(buddyAllocator->head, buddyAllocator->tail,
                                  totalSize);
    }

    if (found != NULL) {
        found->isFree = false;
        void *result = (void *)((char *)found + FLO_SIZEOF(flo_BuddyBlock));
        if (FLO_ZERO_MEMORY & flags) {
            memset(result, 0, total);
        }
        return result;
    }

    FLO_ASSERT(false);
    if (flags & FLO_NULL_ON_FAIL) {
        return NULL;
    }
    __builtin_longjmp(buddyAllocator->jmp_buf, 1);
}

void flo_freeBuddy(flo_BuddyAllocator *buddyAllocator, void *data) {
    FLO_ASSERT((void *)buddyAllocator->head <= data);
    FLO_ASSERT(data < (void *)buddyAllocator->tail);

    flo_BuddyBlock *block =
        (flo_BuddyBlock *)((char *)data - FLO_SIZEOF(flo_BuddyBlock));
    block->isFree = true;

    flo_coalesceBuddies(buddyAllocator->head, buddyAllocator->tail);
}
