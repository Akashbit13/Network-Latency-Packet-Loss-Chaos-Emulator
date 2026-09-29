#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {

    // Total packets expected from the client
    int totalPackets = 100;

    // Create a UDP socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Create server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to port 8080
    if (bind(serverSocket,
             (struct sockaddr*)&serverAddress,
             sizeof(serverAddress)) < 0) {

        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    std::cout << "UDP Server started on port 8080." << std::endl;
    std::cout << "Waiting for packets..." << std::endl;

    char buffer[1024];

    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);

    int receivedPackets = 0;

    // Receive packets
    for (int i = 0; i < totalPackets; i++) {

        int bytesReceived = recvfrom(
            serverSocket,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr*)&clientAddress,
            &clientLength
        );

        if (bytesReceived < 0) {
            perror("Receiving data failed");
            break;
        }

        // Add string ending
        buffer[bytesReceived] = '\0';

        receivedPackets++;

        std::cout << "Received: " << buffer << std::endl;
    }

    // Calculate lost packets
    int lostPackets = totalPackets - receivedPackets;

    // Calculate packet loss percentage
    double packetLoss = (lostPackets * 100.0) / totalPackets;

    std::cout << std::endl;
    std::cout << "========== Network Test Results ==========" << std::endl;
    std::cout << "Packets Sent     : " << totalPackets << std::endl;
    std::cout << "Packets Received : " << receivedPackets << std::endl;
    std::cout << "Packets Lost     : " << lostPackets << std::endl;
    std::cout << "Packet Loss      : " << packetLoss << "%" << std::endl;
    std::cout << "===========================================" << std::endl;

    // Close the socket
    close(serverSocket);

    return 0;
}
