#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    cn_grayscale = 1,
    cn_rgb = 3
} channel;

typedef struct image_mat_structure
{
    unsigned char *data;
    unsigned int width;
    unsigned int height;
    channel cn;
} *image_mat;

/// @brief Сообщение об ошибке
void print_error(const char *message);

/// @brief Сообщение в лог
void print_log(const char *message);

/// @brief Создание изображения
image_mat new_image(unsigned int width, unsigned int height, channel channels);

/// @brief Освобождение изображения
void free_image(image_mat image);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_H_INCLUDED
