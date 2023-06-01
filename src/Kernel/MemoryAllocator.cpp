#include "../../h/Kernel/MemoryAllocator.hpp"
#include "../../lib/mem.h"

Block* MemoryAllocator::freeBlocksList = nullptr;

size_t MemoryAllocator::align(size_t size)
{
    return ((size - 1) / MEM_BLOCK_SIZE + 1) * MEM_BLOCK_SIZE;
}

void MemoryAllocator::initFirstBlock()
{
    auto firstBlock = (Block*)HEAP_START_ADDR;
    firstBlock->size = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR + 1;
    firstBlock->next = nullptr;
    freeBlocksList = firstBlock;
}

Block* MemoryAllocator::splitFreeBlockAndUpdateFreeList(Block *block, size_t neededSize)
{
    if(neededSize % MEM_BLOCK_SIZE != 0 || neededSize > block->size) return 0;

    if(block->size - neededSize > 0)
    {
        auto leftoverBlockAddress = (char*)block + neededSize;
        auto leftoverBlock = (Block*)leftoverBlockAddress;
        leftoverBlock->size = block->size - neededSize;
        leftoverBlock->prev = block->prev;
        leftoverBlock->next = block->next;

        if(block->next != nullptr) block->next->prev = leftoverBlock;
        if(block->prev != nullptr) block->prev->next = leftoverBlock;
        else freeBlocksList = leftoverBlock;

        block->size = neededSize;
        block->next = leftoverBlock;
    }

    // Remove the allocated block from the list
    if(block->prev != nullptr) block->prev->next = block->next;
    if(block->next != nullptr) block->next->prev = block->prev;
    if(freeBlocksList == block) freeBlocksList = block->next;

    block->prev = nullptr;
    block->next = nullptr;

    return block;
}

Block* MemoryAllocator::firstFit(size_t minSize)
{
    auto iterator = freeBlocksList;

    while (iterator != nullptr)
    {
        if (iterator->size < minSize)
        {
            iterator = iterator->next;
            continue;
        }

        return iterator;
    }

    return nullptr;
}

Block* MemoryAllocator::mergeBlocks(Block *parent, Block *child)
{
    parent->size += child->size;
    parent->next = child->next;
    if(child->next) child->next->prev = parent;

    return parent;
}

void* MemoryAllocator::alloc(size_t size)
{
    // Can't allocate a block with size 0
    if(size == 0) {
        return nullptr;
    }

    // Include the size of the descriptor and align it to MEM_BLOCK_SIZE
    size += sizeof(Block);
    size = align(size);

    if(freeBlocksList == nullptr) initFirstBlock();

    // Find a suitable free block
    Block* blockToAllocate = firstFit(size);
    // Out of memory
    if(blockToAllocate == nullptr) return nullptr;

    // Leave the rest of the block that we don't need free and update the list of free blocks
    blockToAllocate = splitFreeBlockAndUpdateFreeList(blockToAllocate, size);

    // Return the actual memory pointer after the descriptor
    return (char*)blockToAllocate + sizeof(Block);
}

int MemoryAllocator::free(void *ptr)
{
    // Get the descriptor of the allocated block
    auto descriptor = (Block*)( (char*)ptr - sizeof(Block) );

    // Find where to insert the block in the list
    Block* iterator = freeBlocksList;
    if(iterator == nullptr)
    {
        freeBlocksList = descriptor;
        return 0;
    }

    while(iterator < descriptor)
    {
        // Case where the block to free is the last block
        if(iterator->next == nullptr)
        {
            iterator->next = descriptor;
            descriptor->prev = iterator;
            break;
        }

        iterator = iterator->next;
    }

    // If we try to free an already freed block, return
    if(iterator == descriptor) return 0;

    if(iterator > descriptor)
    {
        descriptor->next = iterator;
        descriptor->prev = iterator->prev;

        if(iterator->prev != nullptr) iterator->prev->next = descriptor;
        else freeBlocksList = descriptor;

        iterator->prev = descriptor;
    }

    // Merge with the block before and after if possible
    Block* blockToMergeWith = descriptor;

    // Check if the previous block ends where the current starts
    if(descriptor->prev && (char*)descriptor->prev + descriptor->prev->size == (char*)descriptor)
    {
        blockToMergeWith = descriptor->prev;
        // If the previous block "eats" the current, then we want to merge the next block with the previous block
        blockToMergeWith = mergeBlocks(blockToMergeWith, descriptor);
    }

    // Check if the next block starts where the current ends
    if(descriptor->next && (char*)descriptor->next == (char*)blockToMergeWith + blockToMergeWith->size)
    {
        mergeBlocks(blockToMergeWith, descriptor->next);
    }

    return 0;
}
