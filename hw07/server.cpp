#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <ctime>
#include <cstring>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HEARTBEAT_INTERVAL 5
#define MONITOR_INTERVAL 5
#define TIMEOUT 15

std::unordered_map<int, std::string> clients;
std::mutex clients_mutex;

std::unordered_map<int, std::chrono::steady_clock::time_point> heartbeat_timestamps;
std::mutex heartbeat_mutex;
std::atomic<bool> stop_server(false);


void broadcastMessage(const std::string &message, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (const auto &client : clients) {
        if (client.first != sender_fd) {
            send(client.first, message.c_str(), message.size(), 0);
        }
    }
}


void monitorHeartbeats(int timeout_seconds) {
    while (!stop_server) {
        std::this_thread::sleep_for(std::chrono::seconds(MONITOR_INTERVAL));
        auto now = std::chrono::steady_clock::now();

        std::lock_guard<std::mutex> lock(heartbeat_mutex);
        for (auto it = heartbeat_timestamps.begin(); it != heartbeat_timestamps.end();) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
            if (duration > timeout_seconds) {
                std::cout << "Client " << it->first << " timed out. Closing connection." << std::endl;
                close(it->first);
                it = heartbeat_timestamps.erase(it);
                broadcastMessage(clients[it->first] + " has left the chat\n", it->first);
            } else {
                ++it;
            }
        }
    }
    std::cout << "Heartbeat monitor thread exiting." << std::endl;
}


void initializeHeartbeat(int client_fd) {
    std::lock_guard<std::mutex> lock(heartbeat_mutex);
    heartbeat_timestamps[client_fd] = std::chrono::steady_clock::now();
}


void updateHeartbeat(int client_fd) {
    std::lock_guard<std::mutex> lock(heartbeat_mutex);
    if (heartbeat_timestamps.find(client_fd) != heartbeat_timestamps.end()) {
        heartbeat_timestamps[client_fd] = std::chrono::steady_clock::now();
    }
}


void handleClient(int client_fd) {
    char buffer[BUFFER_SIZE];
    std::string nickname = "Anonymous";
    auto last_heartbeat = std::chrono::steady_clock::now();

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break; // Connection closed or error
        }

        auto now = std::chrono::steady_clock::now();
        last_heartbeat = now;

        std::string message(buffer);
        if (message.rfind("/nick ", 0) == 0) {
            nickname = message.substr(6);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients[client_fd] = nickname;
            }
            std::string ack = "Nickname set to " + nickname + "\n";
            std::cout << client_fd << " is now known as " << nickname << std::endl;
            send(client_fd, ack.c_str(), ack.size(), 0);
        } else if (message == "/heartbeat") {
            updateHeartbeat(client_fd);
        } else if (message == "/exit") {
            std::string full_message = nickname + " has left the chat\n";
            broadcastMessage(full_message, client_fd);
            break;
        } else {
            std::string full_message = "[" + nickname + "]: " + message;
            broadcastMessage(full_message, client_fd);
        }
    }

    // Remove client on disconnect
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        broadcastMessage(nickname + " has left the chat\n", client_fd);
        std::cout << "Client " << client_fd << " disconnected." << std::endl;
        clients.erase(client_fd);
    }
    close(client_fd);
}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << PORT << std::endl;

    std::thread heartbeat_thread(monitorHeartbeats, TIMEOUT);
    heartbeat_thread.detach();

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        initializeHeartbeat(new_socket);

        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }
        else {
            std::cout << "New client connected: " << new_socket << std::endl;
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients[new_socket] = "Anonymous";
        }

        std::thread client_thread(handleClient, new_socket);
        client_thread.detach();
    }

    return 0;
}
