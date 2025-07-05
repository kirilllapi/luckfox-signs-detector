#include "Segmentation.h"


int32_t Segmentation::ZETTA = 0;
int32_t Segmentation::GAMMA = 0;

void Segmentation::set_adaptive_coff(uint32_t zetta, uint32_t gamma)
{
    Segmentation::ZETTA = zetta;
    Segmentation::GAMMA = gamma;
}

void Segmentation::analize_hists(image_mat image)
{
    uint32_t hist_R[256] = {0};
    uint32_t hist_G[256] = {0};
    uint32_t hist_B[256] = {0};

    uint32_t max_element_hist_R = 0;
    uint32_t max_element_hist_G = 0;
    uint32_t max_element_hist_B = 0;

    uint32_t size_mat = image->height * image->width;

    uint32_t R, G, B;

    // Делаем проход по трем частям гистограммы
    for (uint32_t i = 0; i < size_mat; i++)
    {
        R = (image->data)[i * 3];
        G = (image->data)[i * 3 + 1];
        B = (image->data)[i * 3 + 2];

        hist_R[R]++;
        hist_G[G]++;
        hist_B[B]++;

        if (hist_R[R] > max_element_hist_R) max_element_hist_R = hist_R[R];
        if (hist_G[G] > max_element_hist_G) max_element_hist_G = hist_G[G];
        if (hist_B[B] > max_element_hist_B) max_element_hist_B = hist_B[B];
    }

    float  alpha_R = 0.3 * max_element_hist_R;
    float  alpha_G = 0.3 * max_element_hist_G;
    float  alpha_B = 0.3 * max_element_hist_B;


    // Массивы для хранения доли пикселей в каждой зоне
    uint32_t left_R = 0, mid_R = 0, right_R = 0;
    uint32_t left_G = 0, mid_G = 0, right_G = 0;
    uint32_t left_B = 0, mid_B = 0, right_B = 0;


    uint32_t total_R = 0, total_G = 0, total_B = 0;

    for (int i = 0; i <= 255; i++)
    {
        total_R += hist_R[i];
        if ((float)hist_R[i] >= alpha_R)
         {
            if (i <= 85) left_R += hist_R[i];
            else if (i <= 170) mid_R += hist_R[i];
            else right_R += hist_R[i];
        }

        total_G += hist_G[i];
        if ((float)hist_G[i] >= alpha_G)
        {
            if (i <= 85) left_G += hist_G[i];
            else if (i <= 170) mid_G += hist_G[i];
            else right_G += hist_G[i];
        }

        total_B += hist_B[i];
        if ((float)hist_B[i] >= alpha_B)
        {
            if (i <= 85) left_B += hist_B[i];
            else if (i <= 170) mid_B += hist_B[i];
            else right_B += hist_B[i];
        }
    }


    float  fraction_left_R  =  (float) left_R / total_R;
    float  fraction_mid_R   =  (float) mid_R / total_R;
    float  fraction_right_R =  (float) right_R / total_R;

    float  fraction_left_G  =  (float) left_G / total_G;
    float  fraction_mid_G   =  (float) mid_G / total_G;
    float  fraction_right_G =  (float) right_G / total_G;

    float  fraction_left_B  =  (float) left_B / total_B;
    float  fraction_mid_B   =  (float) mid_B / total_B;
    float  fraction_right_B =  (float) right_B / total_B;


    if (total_R > 0)
    {
        fraction_left_R  =  (float) left_R / total_R;
        fraction_mid_R   =  (float) mid_R / total_R;
        fraction_right_R =  (float) right_R / total_R;
    }

    if (total_G > 0)
    {
        fraction_left_G  =  (float) left_G / total_G;
        fraction_mid_G   =  (float) mid_G / total_G;
        fraction_right_G =  (float) right_G / total_G;
    }

    if (total_B > 0)
    {
        fraction_left_B  =  (float) left_B / total_B;
        fraction_mid_B   =  (float) mid_B / total_B;
        fraction_right_B =  (float) right_B / total_B;
    }



    uint8_t max_zone[3] =
    {
        (fraction_left_R > fraction_mid_R && fraction_left_R > fraction_right_R) ? 1 :
        (fraction_mid_R > fraction_left_R && fraction_mid_R > fraction_right_R) ? 2 : 3,

        (fraction_left_G > fraction_mid_G && fraction_left_G > fraction_right_G) ? 1 :
        (fraction_mid_G > fraction_left_G && fraction_mid_G > fraction_right_G) ? 2 : 3,

        (fraction_left_B > fraction_mid_B && fraction_left_B > fraction_right_B) ? 1 :
        (fraction_mid_B > fraction_left_B && fraction_mid_B > fraction_right_B) ? 2 : 3
    };

    printf("max zone R = %d; max zone G = %d; max zone B = %d;\n Sum = %d\n", max_zone[0], max_zone[1], max_zone[2], max_zone[0] + max_zone[1] + max_zone[2]);


    // Определяем освещение
    if (max_zone[0] + max_zone[1] + max_zone[2] == 9)
    {
        set_adaptive_coff(45, 25);
    }
    else if (max_zone[0] + max_zone[1] + max_zone[2] == 8)
    {
        set_adaptive_coff(40, 25);
    }
    else if (max_zone[0] + max_zone[1] + max_zone[2] == 7)
    {
        set_adaptive_coff(35, 25);
    }
    else if (max_zone[0] + max_zone[1] + max_zone[2] == 6)
    {
        set_adaptive_coff(30, 25);
    }
    else if (max_zone[0] + max_zone[1] + max_zone[2] == 5)
    {
        set_adaptive_coff(25, 25);
    }
    else if (max_zone[0] + max_zone[1] + max_zone[2] == 4)
    {
        set_adaptive_coff(20, 25);
    }
    else
    {
        set_adaptive_coff(15, 20);
    }
}



// NOTE =============== IHSL ===================
void Segmentation::rgb_to_ihsl_mask(const image_mat image_src, image_mat image_dst)
{
    uint32_t size_mat = image_src->height * image_src->width;

    // Объявление переменных
    uint8_t H, S, L;
    uint8_t R, G, B;

    // Угол в цветовом пространстве HSL;
    double theta;
    double denominator;
    double argument;


    // Максимально и минимальное значение пикселей в каналах
    uint8_t max, min;


    for (uint32_t i = 0; i < size_mat; i++)
    {

        R = (image_src->data)[i * 3];
        G = (image_src->data)[i * 3 + 1];
        B = (image_src->data)[i * 3 + 2];

        denominator = sqrt((double)R*R + (double)G*G + (double)B*B - (double)R*G - (double)R*B - (double)G*B);
        argument = ((double)R - (double)G / 2.0 - (double)B / 2.0) / denominator;

        if (argument < -1.0) argument = -1.0;
        if (argument > 1.0) argument = 1.0;

        // Нахождение угла
        theta = acos(argument);

        // Находждение Hue
        if (B <= G)
        {
            H = (uint8_t) theta * (180.0 / M_PI);
        }
        else
        {
            H = (uint8_t) (360 - theta * (180.0 / M_PI));
        }

        if (argument < -1.0) argument = -1.0;
        if (argument > 1.0) argument = 1.0;

        // Находждение максимального значения пикселя из 3-х каналов
        max = (R > G ? (R > B ? R : B) : (G > B ? G : B));
        min = (R < G ? (R < B ? R : B) : (G < B ? G : B));

        // Нахождение Saturation
        S = max - min;

        // Нахождение luminance
        L = (uint8_t)(0.212 * R + 0.715 * G + 0.072 * B);

        if (max == G || ((abs(R - G) < ZETTA) && (abs(B - G) < GAMMA)))
        {
            (image_dst->data)[i] = 0;
        }
        else
        {
            // Сохраняем значения в HSL
            // (image_dst->data)[i * 3] = H;
            // (image_dst->data)[i * 3 + 1]  = S;
            // (image_dst->data)[i * 3 + 2]  = L;

            // Сохраняем значения в RGB
            (image_dst->data)[i] = 255;
        }
    }
}
