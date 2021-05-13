#include "MemoryManager.h"

PATTERN_SINGLETON_IMPLEMENT(CMemoryManager)

CMemoryManager::CMemoryManager()
{

}

CMemoryManager::~CMemoryManager()
{

}

void CMemoryManager::showMemory()
{
    tracef("current total used memory is 512 MB\n");
}
