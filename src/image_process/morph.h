#ifndef MORPH_H
#define MORPH_H

#include "common/Defs.h"
#include "memory/Tensor.h"

// dilate 膨胀
bool dilate(const CTensor<uint8>& src, CTensor<uint8>& dst, const CTensor<uint8>& kernel);

// erode 腐蚀
bool erode(const CTensor<uint8>& src, CTensor<uint8>& dst, const CTensor<uint8>& kernel);

struct Contour
{
    Point start_point;
    std::vector<int> list;
};

bool findContours(const CTensor<uint8>& src, std::vector<Contour>& contours);

#endif // MORPH_H
