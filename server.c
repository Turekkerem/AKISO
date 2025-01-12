#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 1402

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[1024];
    struct sockaddr_in serv_addr, cli_addr;

    // Utworzenie gniazda (socket)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // Przygotowanie adresu serwera
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Powiązanie gniazda z adresem
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // Nasłuchiwanie na porcie
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Akceptowanie połączeń przychodzących
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) {
        error("ERROR on accept");
    }

    // Odczyt zapytania (choć nie będziemy go analizować)
    bzero(buffer, 1024);
    read(newsockfd, buffer, 1023);

    // Otwieranie pliku site.html
    FILE *file = fopen("site.html", "r");
    if (file == NULL) {
        error("ERROR opening file");
    }

    // Wczytanie zawartości pliku do bufora
    char html_content[2048];
    size_t len = fread(html_content, 1, sizeof(html_content) - 1, file);
    html_content[len] = '\0';  // Dodanie znaków końca ciągu

    fclose(file);

    // Wysłanie nagłówków HTTP
    char response[4096];
    snprintf(response, sizeof(response),
             "HTTP/1.0 200 OK\nContent-Type: text/html\n\n%s", html_content);

    // Wysłanie odpowiedzi
    write(newsockfd, response, strlen(response));

    // Zamknięcie połączenia
    close(newsockfd);
    close(sockfd);

    return 0;
}

