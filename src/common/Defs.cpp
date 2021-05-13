#include "Defs.h"
#include <stdio.h>
#include <stdarg.h>

std::vector<int> creatVector(int n, ...)
{
    std::vector<int> ret;

    va_list list;
    va_start(list, n);
    for (int i=0; i<n; ++i)
    {
        ret.push_back(va_arg(list, int));
    }
    va_end(list);

    return ret;
}
