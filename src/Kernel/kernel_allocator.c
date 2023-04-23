//
// Kernel implementation of heap memory allocation
// Helper funcitons marked as static to limit their linking to this translation unit
//

#include "../../lib/hw.h"

typedef struct Block
{
    size_t size;
    struct Block* prev;
    struct Block* next;
} Block;

static Block* freeBlocksList = 0;

// Make the first free block (which will be the whole heap) on first allocation.
static void initFirstBlock()
{
    Block* firstBlock = (Block*)HEAP_START_ADDR;
    firstBlock->size = (HEAP_END_ADDR - 1) - HEAP_START_ADDR;
    firstBlock->next = 0;
    freeBlocksList = firstBlock;
}

// Align given size to MEM_BLOCK_SIZE
inline static size_t align(size_t size)
{
    return ((size - 1) / MEM_BLOCK_SIZE + 1) * MEM_BLOCK_SIZE;
}

// Get a block of needed size from a free block and leave the rest free. Needed size includes the descriptor size.
static Block* splitFreeBlockAndUpdateFreeList(Block* block, size_t neededSize)
{
    if(neededSize % MEM_BLOCK_SIZE != 0 || neededSize > block->size) return 0;

    if(block->size - neededSize >= MEM_BLOCK_SIZE)
    {
        void* leftoverBlockAddress = (void*)block + neededSize;
        Block* leftoverBlock = (Block*)leftoverBlockAddress;
        leftoverBlock->size = block->size - neededSize;
        leftoverBlock->prev = block->prev;
        leftoverBlock->next = block->next;
        block->size = neededSize;
        block->next = leftoverBlock;
    }

    // Remove the allocated block from the list
    if(block->prev != 0) block->prev->next = block->next;
    if(block->next != 0) block->next->prev = block->prev;
    if(freeBlocksList == block) freeBlocksList = block->next;

    block->prev = 0;
    block->next = 0;

    return block;
}

// Find the first block in the list of free blocks that is big enough to fit minSize.
static Block* firstFit(size_t minSize)
{
    Block* iterator = freeBlocksList;

    while (iterator != 0)
    {
        if (iterator->size < minSize)
        {
            iterator = iterator->next;
            continue;
        }

        return iterator;
    }

    return 0;
}

// Merges child into parent, returns merged block (parent block)
static Block* mergeBlocks(Block* parent, Block* child)
{
    parent->size += child->size;
    parent->next = child->next;
    if(child->next) child->next->prev = parent;

    return parent;
}

// Allocate a memory block of "size" bytes on the heap.
void* kernel_alloc(size_t size)
{
    // Can't allocate a block with size 0
    if(size == 0) return 0;

    // Include the size of the descriptor and align it to MEM_BLOCK_SIZE
    size += sizeof(Block);
    size = align(size);

    if(freeBlocksList == 0) initFirstBlock();

    // Find a suitable free block
    Block* blockToAllocate = firstFit(size);
    // Out of memory
    if(blockToAllocate == 0) return 0;

    // Leave the rest of the block that we don't need free and update the list of free blocks
    blockToAllocate = splitFreeBlockAndUpdateFreeList(blockToAllocate, size);

    // Return the actual memory pointer after the descriptor
    return (void*)blockToAllocate + sizeof(Block);
}

// Free memory allocated by kernel_alloc
int kernel_free(void* ptr)
{
    // Get the descriptor of the allocated block
    Block* descriptor = (Block*)(ptr - sizeof(Block));

    // Find where to insert the block in the list
    Block* iterator = freeBlocksList;

    while(iterator < descriptor)
    {
        // Case where the block to free is the last block
        if(iterator->next == 0)
        {
            iterator->next = descriptor;
            break;
        }

        iterator = iterator->next;
    }

    if(iterator > descriptor)
    {
        descriptor->next = iterator;
        descriptor->prev = iterator->prev;

        if(iterator->prev) iterator->prev->next = descriptor;
        else freeBlocksList = descriptor;

        iterator->prev = descriptor;
    }

    // Merge with the block before and after if possible
    Block* blockToMergeWith = descriptor;

    // Check if the previous block ends where the current starts
    if(descriptor->prev && (void*)descriptor->prev + descriptor->prev->size == (void*)descriptor)
    {
        blockToMergeWith = descriptor->prev;
        // If the previous block "eats" the current, then we want to merge the next block with the previous block
        blockToMergeWith = mergeBlocks(blockToMergeWith, descriptor);
    }

    // Check if the next block starts where the current ends
    if(descriptor->next && (void*)descriptor->next == (void*)blockToMergeWith + blockToMergeWith->size)
    {
        mergeBlocks(blockToMergeWith, descriptor->next);
    }

    return 0;
}
