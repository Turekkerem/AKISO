#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define PART_SIZE 512

void handle_client(int client_sock, char *html_content);

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Otwórz plik HTML
    FILE *file = fopen("szachy.html", "r");
    if (!file) {
        perror("Nie można otworzyć pliku HTML");
        return EXIT_FAILURE;
    }

    // Załaduj zawartość HTML do pamięci
   fseek(file, 0, SEEK_END);
   long html_len = ftell(file);
   fseek(file, 0, SEEK_SET);
//	long html_len = sizeof(file);
    char *html_content = malloc(html_len + 1);
    if (!html_content) {
        perror("Nie udało się przydzielić pamięci");
        fclose(file);
        return EXIT_FAILURE;
    }

    fread(html_content, 1, html_len, file);
    fclose(file);
    html_content[html_len] = '\0';

    // Utwórz gniazdo
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Błąd otwierania gniazda");
        free(html_content);
        return EXIT_FAILURE;
    }

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Konfiguracja adresu serwera
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Powiązanie gniazda
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd podczas powiązania");
        free(html_content);
        close(server_sock);
        return EXIT_FAILURE;
    }

    // Nasłuchiwanie na połączenia
    if (listen(server_sock, 5) < 0) {
        perror("Błąd podczas nasłuchiwania");
        free(html_content);
        close(server_sock);
        return EXIT_FAILURE;
    }

    printf("Serwer nasłuchuje na porcie %d\n", PORT);

    while (1) {
        // Akceptacja połączenia
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Błąd podczas akceptacji połączenia");
            continue;
        }

        // Obsługa klienta
        handle_client(client_sock, html_content);
    }

    free(html_content);
    close(server_sock);
    return 0;
}

void handle_client(int client_sock, char *html_content) {
    // Nagłówki HTTP
    const char *response_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";

    // Wysłanie nagłówków
    send(client_sock, response_header, strlen(response_header), 0);

    // Rozbicie i wysyłanie zawartości HTML partiami
    size_t total_len = strlen(html_content);
    size_t sent_bytes = 0;

    while (sent_bytes < total_len) {
        size_t remaining_bytes = total_len - sent_bytes;
        size_t chunk_size = remaining_bytes > PART_SIZE ? PART_SIZE : remaining_bytes;

        if (send(client_sock, html_content + sent_bytes, chunk_size, 0) < 0) {
            perror("Błąd podczas wysyłania danych");
            break;
        }
        sent_bytes += chunk_size;
    }

    close(client_sock);
}

