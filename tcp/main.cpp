#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <iostream>

using namespace cv;

#define TCP_SRV_PORT 5005  // Порт для приема
const int BUFFER_SIZE = 4096;  // Размер буфера для приема данных

int main() {
    // Создаем TCP-сокет
    int sock_serv = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_serv < 0) {
        perror("Ошибка при создании сокета");
        return -1;
    }

    // Разрешаем повторное использование порта
    int opt = 1;
    setsockopt(sock_serv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Настраиваем адрес сервера
    struct sockaddr_in addr_serv;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_port = htons(TCP_SRV_PORT);
    addr_serv.sin_addr.s_addr = INADDR_ANY;

    // Привязываем сокет
    if (bind(sock_serv, (struct sockaddr*)&addr_serv, sizeof(addr_serv)) < 0) {
        perror("Ошибка при привязке сокета");
        close(sock_serv);
        return -1;
    }

    // Слушаем входящие соединения (максимум 5 клиентов в очереди)
    if (listen(sock_serv, 5) < 0) {
        perror("Ошибка при переводе в режим прослушивания");
        close(sock_serv);
        return -1;
    }

    std::cout << "Сервер запущен, жду подключения..." << std::endl;

    // Принимаем соединение от клиента
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sock = accept(sock_serv, (struct sockaddr*)&client_addr, &client_len);
    if (client_sock < 0) {
        perror("Ошибка при принятии соединения");
        close(sock_serv);
        return -1;
    }

    std::cout << "Клиент подключен: " << inet_ntoa(client_addr.sin_addr) << std::endl;

    uint32_t imgSize;

    while (true)
    {
        std::vector<cv::Mat> images;

        for (int i = 0; i < 2; ++i)
        {
            // Получаем размер изображения
            uint32_t imgSizeNet;
            ssize_t receivedSize = recv(client_sock, &imgSizeNet, sizeof(imgSizeNet), 0);
            if (receivedSize != sizeof(imgSizeNet)) {
                std::cerr << "Ошибка при приеме размера изображения!" << std::endl;
                break;
            }

            uint32_t imgSize = ntohl(imgSizeNet);
            std::cout << "Изображение " << (i + 1) << " — размер: " << imgSize << " байт" << std::endl;

            if (imgSize > 900000) {
                std::cerr << "Слишком большое изображение!" << std::endl;
                break;
            }

            std::vector<uchar> imgData(imgSize);
            size_t receivedTotal = 0;

            while (receivedTotal < imgSize) {
                ssize_t receivedNow = recv(client_sock, imgData.data() + receivedTotal,
                                           std::min(BUFFER_SIZE, static_cast<int>(imgSize - receivedTotal)), 0);
                if (receivedNow <= 0) {
                    std::cerr << "Ошибка при приеме данных изображения!" << std::endl;
                    break;
                }
                receivedTotal += receivedNow;
            }

            if (receivedTotal != imgSize) {
                std::cerr << "Не удалось получить полное изображение!" << std::endl;
                break;
            }

            cv::Mat img = cv::imdecode(imgData, IMREAD_COLOR);
            if (img.empty()) {
                std::cerr << "Ошибка декодирования изображения!" << std::endl;
                break;
            }

            images.push_back(img);

        }

        // Если оба изображения успешно получены, отображаем
        if (images.size() == 2) {
            //cv::imshow("ZXING", images[0]);
            cv::imshow("BITMAP", images[1]);
            cv::imshow("CAMERA", images[0]);

        }

        if (cv::waitKey(1) == 27) break;
    }

    // Закрываем соединение
    close(client_sock);
    close(sock_serv);
    return 0;
}
