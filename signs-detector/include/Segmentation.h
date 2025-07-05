#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "Image.h"
#include <cstdint>  // Для uint32_t, int32_t, uint8_t и т.д.
#include <cstring>  // Для memcpy
#include <cmath>    // Для sqrt, fminf, fmaxf

class Segmentation
{
    public:

        static int32_t ZETTA;
        static int32_t GAMMA;

        static void set_adaptive_coff(uint32_t, uint32_t);
        static void analize_hists(image_mat);
        static void rgb_to_ihsl_mask(const image_mat, image_mat);
};

#endif // SEGMENTATION_H
