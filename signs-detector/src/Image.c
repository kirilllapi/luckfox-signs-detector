#include "Image.h"
#include <stdlib.h>

void print_error(const char *message)
{
    fprintf(stdout, "%s\n", message);
    exit(EXIT_FAILURE);
}

void print_log(const char *message)
{
    fprintf(stdout, "%s\n", message);
}

image_mat new_image(unsigned int width, unsigned int height, channel channels)
{
    if (width == 0 || height == 0 || channels == 0)
        print_error("new_image: invalid input parameters.");

    image_mat image = (image_mat)malloc(sizeof(struct image_mat_structure));
    if (image == NULL)
        print_error("new_image: not enough memory.");

    image->width = width;
    image->height = height;
    image->cn = channels;

    image->data = (unsigned char *)calloc(width * height * channels, sizeof(unsigned char));
    if (image->data == NULL)
        print_error("new_image: not enough memory.");

    return image;
}

void free_image(image_mat image)
{
    if (image == NULL || image->data == NULL)
        print_error("image_mat: invalid input image_mat.");

    free(image->data);
    free(image);
}
