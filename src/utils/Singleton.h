#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <stdio.h>
#include <stdlib.h>

#include "common/Defs.h"

#undef PATTERN_SINGLETON_DECLARE
#define PATTERN_SINGLETON_DECLARE(classname)    \
public:                                         \
    static classname* instance();               \
private:                                        \
    classname();                                \
    ~classname();                               \
friend class std::auto_ptr<classname>;          \


#undef PATTERN_SINGLETON_IMPLEMENT
#define PATTERN_SINGLETON_IMPLEMENT(classname)                  \
static CMutex sm_mutex##classname;                              \
static std::auto_ptr<classname> instance##classname(nullptr);   \
void exit##classname(void)                                      \
{                                                               \
    instance##classname = std::auto_ptr<classname>(nullptr);    \
}                                                               \
classname* classname::instance()                                \
{                                                               \
    if (nullptr == instance##classname.get())                   \
    {                                                           \
        CGuard lock(sm_mutex##classname);                       \
        if (nullptr == instance##classname.get())               \
        {                                                                   \
            instance##classname = std::auto_ptr<classname>(new classname);  \
            if (0 != atexit(exit##classname))                               \
            {                                                               \
                errorf("instance%s, atexit failed\n", #classname);          \
            }                                                   \
        }                                                       \
    }                                                           \
    return instance##classname.get();                           \
}                                                               \

#endif // SINGLETON_H
