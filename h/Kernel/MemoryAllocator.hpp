#ifndef _Memory_Allocator_hpp_
#define _Memory_Allocator_hpp_

#include "../../lib/hw.h"

struct Block
{
    size_t size;
    struct Block* prev;
    struct Block* next;
};

class MemoryAllocator
{
public:
    static void* alloc(size_t size);
    static int free(void* ptr);

private:
    inline static size_t align(size_t size);
    static void initFirstBlock();
    static Block* splitFreeBlockAndUpdateFreeList(Block* block, size_t neededSize);
    static Block* firstFit(size_t minSize);
    static Block* mergeBlocks(Block* parent, Block* child);

private:
    static volatile Block* volatile freeBlocksList;
};

#endif // _Memory_Allocator_hpp_
