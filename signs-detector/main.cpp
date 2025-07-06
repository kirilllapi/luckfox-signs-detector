#include <iostream>
#include "Image.h"
#include "ProcessingImage.h"
#include "Segmentation.h"
#include <pthread.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include "tcp.hpp"

using namespace cv;
using namespace std;

int min_area = 300;
int max_area = 1000;
cv::Mat image_mat_to_cvmat(image_mat img)
{
    if (!img || !img->data)
    {
        fprintf(stderr, "image_mat_to_cvmat: недопустимый указатель на image_mat\n");
        return cv::Mat();
    }

    int type;

    switch (img->cn)
    {
        case 1: type = CV_8UC1; break; // Grayscale
        case 3: type = CV_8UC3; break; // BGR (стандарт OpenCV)
        case 4: type = CV_8UC4; break; // BGRA
        default:
            fprintf(stderr, "image_mat_to_cvmat: неподдерживаемое количество каналов: %d\n", img->cn);
            return cv::Mat();
    }

    // Создаём cv::Mat, указывая, что он использует существующий буфер (без копирования)
    return cv::Mat(img->height, img->width, type, img->data).clone(); // clone — чтобы сделать безопасную копию
}

std::vector<cv::Rect> find_objects_bboxes(const cv::Mat &mask)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    std::vector<cv::Rect> bboxes;

    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour);
        if (area < min_area || area > max_area)
            continue;

        bboxes.push_back(cv::boundingRect(contour));
    }

    return bboxes;
}


// Настройка входного изображения
int frame_H = 640;
int frame_W = 480;

int main()
{
    VideoCapture cap;
    cap.set(CAP_PROP_FRAME_WIDTH, frame_W);
    cap.set(CAP_PROP_FRAME_HEIGHT, frame_H);
    cap.open(0);

    const int w = cap.get(CAP_PROP_FRAME_WIDTH);
    const int h = cap.get(CAP_PROP_FRAME_HEIGHT);
    fprintf(stderr, "\n\nFrame res.: %d x %d" , w, h);


    // Инициализация клиента
    int sock;
    if (client_init(&sock))
    {
        return -1;
    }

    Mat bgr;

    while(1)
    {
        cap >> bgr;

        if (bgr.empty())
        {
            fprintf(stderr, RED "Кадр пустой!\n" RESET);
            continue;
        }

        image_mat image_src = new_image(frame_W, frame_H, cn_rgb);
        memcpy(image_src->data, bgr.data, frame_W * frame_H * 3);

        Segmentation::analize_hists(image_src);

        image_mat ihsl_mask = new_image(image_src->width, image_src->height, cn_grayscale);
        image_mat image_seg = new_image(image_src->width, image_src->height, cn_rgb);

        Segmentation::rgb_to_ihsl_mask(image_src, ihsl_mask);

        Segmentation::apply_mask(image_src, ihsl_mask, &image_seg);



        cv::Mat mask_mat = image_mat_to_cvmat(ihsl_mask);
        cv::Mat original_mat = image_mat_to_cvmat(image_src);

        auto bboxes = find_objects_bboxes(mask_mat);

        for (const auto& box : bboxes)
        {
            cv::rectangle(original_mat, box, cv::Scalar(0, 255, 0), 2);
        }

        send_image(original_mat, &sock);

        free_image(image_seg);
        free_image(ihsl_mask);
        free_image(image_src);
    }

    sleep(1);
    cap.release();
    return 0;
}
