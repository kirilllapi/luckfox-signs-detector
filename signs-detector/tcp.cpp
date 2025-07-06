#include "tcp.hpp"


struct sockaddr_in addr;


int client_init(int *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock < 0)
    {
        perror(RED "\nОшибка создания сокета" RESET);
        return 1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(TCP_HOST_PORT);

    if (inet_pton(AF_INET, DEST_IP, &addr.sin_addr) <= 0)
    {
        perror(RED "\ninet_pton failed" RESET);
        return 1;
    }

    if (connect(*sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror(RED "\nОшибка подключения к серверу" RESET);
        return 1;
    }

    return 0;
}


void send_image(cv::Mat bgr, int *sock)
{
    std::vector<uchar> buffer;
    imencode(".jpg", bgr, buffer);

    if (!buffer.empty())
    {
        uint32_t imgSize = buffer.size();
        uint32_t imgSize_net = htonl(imgSize);  // Преобразуем размер в сетевой порядок байтов

        // Отправка размера изображения
        ssize_t sent = send(*sock, &imgSize_net, sizeof(imgSize_net), 0);
        if (sent < 0)
        {
            printf(RED "\nОшибка при отправке размера изображения!" RESET);
            return;
        }

        // Отправка изображения
        size_t totalSent = 0;
        while (totalSent < buffer.size())
        {
            ssize_t sentData = send(*sock, buffer.data() + totalSent, buffer.size() - totalSent, 0);
            if (sentData < 0)
            {
                printf(RED "\nОшибка при отправке пакета!" RESET);
                return;
            }
            totalSent += sentData;
        }
    }
    else
    {
        printf(RED "\nОшибка: Буфер пуст!" RESET);
    }
}
