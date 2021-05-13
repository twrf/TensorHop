#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "common/Defs.h"

class CMemoryManager
{
    PATTERN_SINGLETON_DECLARE(CMemoryManager)

public:

    void showMemory();
};

#endif // MEMORYMANAGER_H
