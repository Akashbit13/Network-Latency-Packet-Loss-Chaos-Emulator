#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {

    // Create a UDP socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Create server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Send 100 packets
    for (int i = 1; i <= 100; i++) {

        char message[100];

        snprintf(message, sizeof(message), "Packet %d", i);

        int bytesSent = sendto(
            clientSocket,
            message,
            strlen(message),
            0,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)
        );

        if (bytesSent < 0) {
            perror("Sending data failed");
            close(clientSocket);
            return 1;
        }

        std::cout << "Sent: " << message << std::endl;
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
