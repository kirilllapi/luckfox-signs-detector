#ifndef TCP_H_INCLUDED
#define TCP_H_INCLUDED


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


//NOTE ============ Константы для вывода цветного текста ============

#define RED     "\x1b[38;5;196m"
#define YELLOW "\x1b[38;5;11m"
#define GREEN "\x1b[38;5;46m"
#define PURPLE "\x1b[38;5;201m"
#define RESET   "\x1b[0m"

//NOTE ============ Перемещение в начало и очистка экрана ============

#define CLEAR_SCREEN "\x1b[H\x1b[J"


//NOTE ============ Настройка клиента ============

#define TCP_HOST_PORT 5005  // Порт для отправки
#define DEST_IP "192.168.0.16"  // IP ПК


//NOTE ============ Функции и переменные для передачи изображений по TCP ============

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

int client_init(int *);
void send_image(cv::Mat, int *);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus


#endif // TCP_H_INCLUDED
