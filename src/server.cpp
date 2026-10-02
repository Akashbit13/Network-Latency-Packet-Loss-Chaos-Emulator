#include <iostream>
#include <string>
#include <cstdio>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int MAX_CHUNKS = 100;

int main() {

    // Create UDP socket
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (serverSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Server address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to port 8080
    if (bind(
            serverSocket,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)) < 0) {

        perror("Bind failed");
        close(serverSocket);
        return 1;
    }

    std::cout << "UDP Server started on port 8080."
              << std::endl;

    std::cout << "Waiting for network test..."
              << std::endl;

    char buffer[1024];

    sockaddr_in clientAddress{};
    socklen_t clientLength = sizeof(clientAddress);

    std::string receivedChunks[MAX_CHUNKS];
    bool chunkReceived[MAX_CHUNKS] = {};

    int totalChunks = 0;
    int receivedChunksCount = 0;
    int receivedBytes = 0;

    while (true) {

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

        buffer[bytesReceived] = '\0';

        std::string data(buffer);

        // DATA|chunk number|total chunks|data
        if (data.rfind("DATA|", 0) == 0) {

            size_t firstSeparator =
                data.find('|', 5);

            size_t secondSeparator =
                data.find('|', firstSeparator + 1);

            if (firstSeparator == std::string::npos ||
                secondSeparator == std::string::npos) {
                continue;
            }

            int chunkNumber = std::stoi(
                data.substr(
                    5,
                    firstSeparator - 5
                )
            );

            totalChunks = std::stoi(
                data.substr(
                    firstSeparator + 1,
                    secondSeparator - firstSeparator - 1
                )
            );

            std::string chunk =
                data.substr(secondSeparator + 1);

            int index = chunkNumber - 1;

            if (index >= 0 && index < MAX_CHUNKS) {

                if (!chunkReceived[index]) {

                    receivedChunks[index] = chunk;
                    chunkReceived[index] = true;

                    receivedChunksCount++;
                    receivedBytes += chunk.length();

                    std::cout << "\nData Unit "
                              << chunkNumber
                              << " received: "
                              << chunk
                              << std::endl;
                }
            }

            // Send acknowledgement
            std::string acknowledgement =
                "ACK|" +
                std::to_string(chunkNumber);

            sendto(
                serverSocket,
                acknowledgement.c_str(),
                acknowledgement.length(),
                0,
                (struct sockaddr*)&clientAddress,
                clientLength
            );
        }

        // End of test
        else if (data.rfind("END|", 0) == 0) {

            std::cout << "\n";
            std::cout << "========== SERVER TEST RESULT =========="
                      << std::endl;

            // Show every data unit
            for (int i = 0;
                 i < totalChunks && i < MAX_CHUNKS;
                 i++) {

                if (chunkReceived[i]) {

                    std::cout << "Data Unit "
                              << i + 1
                              << " : RECEIVED -> "
                              << receivedChunks[i]
                              << std::endl;
                }
                else {

                    std::cout << "Data Unit "
                              << i + 1
                              << " : LOST"
                              << std::endl;
                }
            }

            int lostChunks =
                totalChunks - receivedChunksCount;

            std::cout << "\nReceived Units : "
                      << receivedChunksCount
                      << std::endl;

            std::cout << "Lost Units     : "
                      << lostChunks
                      << std::endl;

            std::cout << "Received Bytes : "
                      << receivedBytes
                      << std::endl;

            // Reconstruct received message
            std::cout << "\nReceived Data  : ";

            for (int i = 0;
                 i < totalChunks && i < MAX_CHUNKS;
                 i++) {

                if (chunkReceived[i]) {
                    std::cout << receivedChunks[i];
                }
                else {
                    std::cout << "[LOST]";
                }
            }

            std::cout << std::endl;

            std::cout << "========================================"
                      << std::endl;

            // Clear data for next test
            for (int i = 0; i < MAX_CHUNKS; i++) {

                receivedChunks[i].clear();
                chunkReceived[i] = false;
            }

            totalChunks = 0;
            receivedChunksCount = 0;
            receivedBytes = 0;
        }
    }

    close(serverSocket);

    return 0;
}
