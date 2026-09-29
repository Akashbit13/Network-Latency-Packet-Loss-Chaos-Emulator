#include <iostream>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>

int main() {
    // Create UDP socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set receive timeout to 1 second
    timeval timeout{};
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(
            clientSocket,
            SOL_SOCKET,
            SO_RCVTIMEO,
            &timeout,
            sizeof(timeout)) < 0) {

        perror("Setting timeout failed");
        close(clientSocket);
        return 1;
    }

    // Create server address
    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    char buffer[1024];

    int totalPackets = 100;
    int receivedPackets = 0;
    int lostPackets = 0;

    double totalLatency = 0.0;
    double minLatency = 0.0;
    double maxLatency = 0.0;

    // Send 100 packets
    for (int i = 1; i <= totalPackets; i++) {

        char message[100];
        snprintf(message, sizeof(message), "Packet %d", i);

        // Start timer
        auto start = std::chrono::steady_clock::now();

        // Send packet
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

        // Wait for server response
        int bytesReceived = recvfrom(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0,
            nullptr,
            nullptr
        );

        // Stop timer
        auto end = std::chrono::steady_clock::now();

        if (bytesReceived < 0) {
            lostPackets++;

            std::cout << "Packet " << i << " - Lost/Timeout"
                      << std::endl;

            continue;
        }

        // Calculate latency
        double latency = std::chrono::duration<double, std::milli>(
            end - start
        ).count();

        receivedPackets++;
        totalLatency += latency;

        // Set first latency as initial min/max
        if (receivedPackets == 1) {
            minLatency = latency;
            maxLatency = latency;
        } else {
            if (latency < minLatency) {
                minLatency = latency;
            }

            if (latency > maxLatency) {
                maxLatency = latency;
            }
        }

        std::cout << "Packet " << i
                  << " - Latency: " << latency << " ms"
                  << std::endl;
    }

    // Calculate packet loss
    double packetLoss = (lostPackets * 100.0) / totalPackets;

    // Calculate average latency
    double averageLatency = 0.0;

    if (receivedPackets > 0) {
        averageLatency = totalLatency / receivedPackets;
    }

    // Display results
    std::cout << std::endl;
    std::cout << "========== Network Test Results ==========" << std::endl;
    std::cout << "Packets Sent     : " << totalPackets << std::endl;
    std::cout << "Packets Received : " << receivedPackets << std::endl;
    std::cout << "Packets Lost     : " << lostPackets << std::endl;
    std::cout << "Packet Loss      : " << packetLoss << "%" << std::endl;
    std::cout << "Average Latency  : " << averageLatency << " ms" << std::endl;
    std::cout << "Minimum Latency  : " << minLatency << " ms" << std::endl;
    std::cout << "Maximum Latency  : " << maxLatency << " ms" << std::endl;
    std::cout << "===========================================" << std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}
