#include "common/Defs.h"
#include "memory/Tensor.h"
#include "format/Bmp.h"
#include "format/Image.h"
#include "image_process/Smooth.h"
#include "image_process/morph.h"
#include "layer/LayerBase.hpp"
#include "net/Net.hpp"

#include <string>
#include <iostream>
#include <signal.h>
#include <execinfo.h>

void SystemErrorHandler(int signum)
{
    const int len=1024;
    void *func[len];
    size_t size;
    char **funs;

    signal(signum, SIG_DFL);
    size = backtrace(func, len);
    funs = (char**)backtrace_symbols(func, size);
    fprintf(stderr, "System error, Stack trace:\n");
    for (unsigned int i=0; i<size; ++i)
    {
        fprintf(stderr, "%d %s \n", i, funs[i]);
    }

    free(funs);
}

int main()
{
    signal(SIGSEGV, SystemErrorHandler);
    signal(SIGABRT, SystemErrorHandler);
    setPrintLevel(logLevelDebug);

    std::shared_ptr<CNet> net(new CNet("./config/mnist/Lenet.json"));

    return 0;
}
