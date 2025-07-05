#ifndef PROCESSINGIMAGE_H
#define PROCESSINGIMAGE_H

#include "Image.h"
#include <cstdint>  // Для uint32_t, int32_t, uint8_t и т.д.
#include <cstring>  // Для memcpy
#include <cmath>    // Для sqrt, fminf, fmaxf

class ProcessingImage
{
    public:
        static void gray_filter(const image_mat, image_mat, unsigned int, unsigned int);
        static void gray_to_rgb(const image_mat, image_mat, unsigned int, unsigned int);
        static void dilate(const image_mat, image_mat, unsigned char);
        static void gausian_blur(const image_mat, image_mat, double);
};

#endif // PROCESSINGIMAGE_H
