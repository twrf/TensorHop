#include <pthread.h>

#include "Mutex.h"
#include "common/Defs.h"

/// pthread_mutexattr_settype bad
//extern "C"  int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind);

struct MutexInternal
{
    pthread_mutex_t mtx;
};

CMutex::CMutex()
{
    m_internal = new MutexInternal;

    int ret = pthread_mutex_init(&m_internal->mtx, nullptr);
    ASSERT(0 == ret);
}

CMutex::~CMutex()
{
    int ret = pthread_mutex_destroy(&m_internal->mtx);
    ASSERT(ret == 0);

    delete m_internal;
    m_internal = nullptr;
}

bool CMutex::enter()
{
    return (pthread_mutex_lock(&m_internal->mtx) == 0);
}

bool CMutex::tryEntry()
{
    return (pthread_mutex_trylock(&m_internal->mtx) == 0);
}

bool CMutex::leave()
{
    return (pthread_mutex_unlock(&m_internal->mtx) == 0);
}


