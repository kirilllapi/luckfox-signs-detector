#include <iostream>
#include "Image.h"
#include "ProcessingImage.h"
#include <pthread.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>

using namespace cv;
using namespace std;

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

    Mat bgr;

    while(1)
    {
        cap >> bgr;
        image_mat image_src = new_image(frame_W, frame_H, cn_rgb);
        memcpy(image_src->data, bgr.data, frame_W * frame_H);

        free_image(image_src);
    }

    sleep(1);
    cap.release();
    return 0;
}
