#ifndef DEFS_H
#define DEFS_H

#include <assert.h>
#include <vector>
#include "utils/PrintLog.h"
#include "utils/Mutex.h"
#include "utils/Singleton.h"
#include "json/json.h"

#ifdef USE_OPENCV
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#endif

#define ASSERT(x) assert(x)

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef char            int8;
typedef short           int16;
typedef int             int32;

#define CV_8U   0
#define CV_8S   1
#define CV_16U  2
#define CV_16S  3
#define CV_32S  4
#define CV_32F  5
#define CV_64F  6
#define CV_USRTYPE1 7

typedef Json::Value CConfigTable;

struct Point
{
public:
    Point()
    {
        x = y = 0;
        width = height = 0;
    }

    Point(const uint32 w, const uint32 h)
    {
        x = y = 0;
        width = w;
        height = h;
    }

    Point(const std::vector<int> sharp)
    {
        x = y = 0;
        width = sharp[0];
        height = sharp[1];
    }

#if 0
	// point[1]
    uint32 operator [] (int index)
    {

    }

	// point[-1, -1]
    uint32 operator [] (int ix, int iy)
    {

    }
#endif

private:
    uint32 x;
    uint32 y;
    uint32 width;
    uint32 height;
};


#endif // DEFS_H
