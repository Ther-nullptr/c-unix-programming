#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void receiveMessages(int server_fd) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(server_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            std::cout << buffer << std::endl;
        } else {
            break;
        }
    }
}

void sendHeartbeats(int server_fd) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        send(server_fd, "/heartbeat", 10, 0);
    }
}

int main() {
    int server_fd;
    struct sockaddr_in server_address;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server. Use /nick <name> to set your nickname." << std::endl;

    std::thread receive_thread(receiveMessages, server_fd);
    std::thread heartbeat_thread(sendHeartbeats, server_fd);

    std::string input;
    while (std::getline(std::cin, input)) {
        send(server_fd, input.c_str(), input.size(), 0);
    }

    receive_thread.join();
    heartbeat_thread.join();
    close(server_fd);

    return 0;
}
