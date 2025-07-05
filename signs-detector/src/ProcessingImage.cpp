#include "ProcessingImage.h"


/// @brief
/// @param src
/// @param dst
/// @param width
/// @param height
void ProcessingImage::gray_filter(const image_mat src, image_mat dst, unsigned int width, unsigned int height)
{
    int pixel_count = width * height;
    for (int i = 0; i < pixel_count; i++)
    {
        int r = (src->data)[i * 3];     // Красный канал
        int g = (src->data)[i * 3 + 1]; // Зеленый канал
        int b = (src->data)[i * 3 + 2]; // Синий канал

        unsigned char gray_pixel = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
        (dst->data)[i] = gray_pixel;
    }
}



/// @brief Преобразует изображение из градаций серого в RGB (дублируя серый оттенок в R, G, B).
/// @param src Исходное изображение в градациях серого.
/// @param dst Целевое изображение в формате RGB.
/// @param width Ширина изображения.
/// @param height Высота изображения.
void ProcessingImage::gray_to_rgb(const image_mat src, image_mat dst, unsigned int width, unsigned int height)
{
    int pixel_count = width * height;
    for (int i = 0; i < pixel_count; i++)
    {
        unsigned char gray_pixel = (src->data)[i]; // Берём значение серого пикселя

        // Копируем его в R, G, B
        (dst->data)[i * 3] = gray_pixel;     // Красный канал
        (dst->data)[i * 3 + 1] = gray_pixel; // Зелёный канал
        (dst->data)[i * 3 + 2] = gray_pixel; // Синий канал
    }
}





void ProcessingImage::dilate(const image_mat image_src, image_mat image_dst, unsigned char number_interation)
{
    uint32_t width = image_src->width;
    uint32_t height = image_src->height;


    for (unsigned char i = 0; i < number_interation; i++)
    {
        for (uint32_t y = 1; y < height; y++)
        {
            for (uint32_t x = 1; x < width; x++)
            {
                if (image_src->data[y * width + x] == 255)
                {
                    image_dst->data[(y - 1) * width + (x - 1)] = 255;
                    image_dst->data[(y - 1) * width + x] = 255;
                    image_dst->data[(y - 1) * width + (x + 1)] = 255;
                    image_dst->data[y * width + (x - 1)] = 255;
                    image_dst->data[y * width + (x + 1)] = 255;
                    image_dst->data[(y + 1) * width + (x - 1)] = 255;
                    image_dst->data[(y + 1) * width + x] = 255;
                    image_dst->data[(y + 1) * width + (x + 1)] = 255;
                }
                else
                {
                    image_dst->data[y * width + x] = 0;
                }
            }
        }
        // Копируем результат обратно в исходное изображение для следующей итерации
        memcpy(image_src->data, image_dst->data, width * height);
    }
}



void ProcessingImage::gausian_blur(const image_mat image_src, image_mat image_dst, double sigma)
{
    int32_t radius = (int32_t)(sqrt((12 * sigma * sigma) / 3) / 2);
    radius = (radius < 1) ? 1 : radius;

    uint8_t size = 2 * radius + 1;

    uint32_t width = image_src->width;
    uint32_t height = image_src->height;

    for (int i = 1; i < image_src->cn + 1; i++)
    {
        // Горизонтальное размытие
        for (uint32_t y = 0; y < height; y++)
        {
            int sum = 0;

            // Начальная сумма
            for (int32_t x = -radius; x <= radius; x++)
            {
                // Преобразуем width к int32_t для сравнения с signed x
                int32_t w = (int32_t)width;

                int32_t clamped_x = (x < 0) ? 0 : ((x >= w) ? (w - 1) : x);
                int32_t index = y * (int32_t)width + clamped_x;

                sum += image_src->data[index * 3 + i];
            }

            // Основной проход
            for (uint32_t x = 0; x < width; x++)
            {
                image_dst->data[(y * width + x) * 3 + i] = (unsigned char)fminf(255, fmaxf(0, sum / size));

                int32_t w = (int32_t)width;
                int32_t xx = (int32_t)x;
                int32_t add_x = (xx + radius + 1 >= w) ? (w - 1) : (xx + radius + 1);
                int32_t sub_x = (xx - radius < 0) ? 0 : (xx - radius);

                int32_t addIndex = y * w + add_x;
                int32_t subIndex = y * w + sub_x;

                sum += image_src->data[addIndex * 3 + i];
                sum -= image_src->data[subIndex * 3 + i];
            }
        }

        image_mat tmp = new_image(width, height, cn_grayscale);

        // Вертикальное размытие
        for (uint32_t x = 0; x < width; x++)
        {
            int sum = 0;

            for (int32_t y = -radius; y <= radius; y++)
            {
                int32_t h = (int32_t)height;
                int32_t clamped_y = (y < 0) ? 0 : ((y >= h) ? (h - 1) : y);

                int32_t index = clamped_y * (int32_t)width + (int32_t)x;
                sum += image_dst->data[index * 3 + i];
            }

            for (uint32_t y = 0; y < height; y++)
            {
                tmp->data[y * width + x] = (unsigned char)fminf(255, fmaxf(0, sum / size));

                int32_t h = (int32_t)height;
                int32_t yy = (int32_t)y;
                int32_t add_y = ((yy + radius + 1) >= h) ? (h - 1) : (yy + radius + 1);
                int32_t sub_y = ((yy - radius) < 0) ? 0 : (yy - radius);

                int32_t addIndex = add_y * (int32_t)width + (int32_t)x;
                int32_t subIndex = sub_y * (int32_t)width + (int32_t)x;

                sum += image_dst->data[addIndex * 3 + i];
                sum -= image_dst->data[subIndex * 3 + i];
            }
        }

        // Последний проход (вертикальный)
        for (uint32_t y = 0; y < height; y++)
        {
            int sum = 0;

            for (int32_t x = -radius; x <= radius; x++)
            {
                int32_t w = (int32_t)width;
                int32_t clamped_x = (x < 0) ? 0 : ((x >= w) ? (w - 1) : x);

                int32_t index = y * w + clamped_x;
                sum += tmp->data[index];
            }

            for (uint32_t x = 0; x < width; x++)
            {
                image_dst->data[(y * width + x) * 3 + i] = (unsigned char)fminf(255, fmaxf(0, sum / size));

                int32_t w = (int32_t)width;
                int32_t xx = (int32_t)x;
                int32_t add_x = (xx + radius + 1 >= w) ? (w - 1) : (xx + radius + 1);
                int32_t sub_x = (xx - radius < 0) ? 0 : (xx - radius);

                int32_t addIndex = y * w + add_x;
                int32_t subIndex = y * w + sub_x;

                sum += tmp->data[addIndex];
                sum -= tmp->data[subIndex];
            }
        }

        free_image(tmp);
    }
}

