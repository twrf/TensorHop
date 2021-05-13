#ifndef MUTEX_H
#define MUTEX_H

#include <assert.h>
#include <stdio.h>

struct MutexInternal;

class CMutex
{
//    CMutex(CMutex const&);
//    CMutex& operator=(CMutex const&);
//    CMutex(const CMutex & ) = default;

public:
    CMutex();

    ~CMutex();

    bool enter();

    bool tryEntry();

    bool leave();

private:
    MutexInternal *m_internal;
};


class CGuard
{
public:
    inline CGuard(CMutex& mutex) : m_mutex(mutex)
    {
        int ret = m_mutex.enter();
        assert(ret);
    }

    inline ~CGuard()
    {
        int ret = m_mutex.leave();
        assert(ret);
    }

private:
    CMutex &m_mutex;
};

#endif // MUTEX_H
