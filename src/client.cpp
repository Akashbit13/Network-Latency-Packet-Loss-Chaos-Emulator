#include <iostream>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <fstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

const int CHUNK_SIZE = 5;

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cout << "Invalid network settings." << std::endl;
        return 1;
    }

    int packetLoss = std::atoi(argv[1]);
    int latency = std::atoi(argv[2]);

    // Create UDP socket
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (clientSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Server address
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    std::string message;

    std::cout << "\nEnter message to transmit: ";

    // Clear the previous newline before getline
    std::cin >> std::ws;
    std::getline(std::cin, message);

    if (message.empty()) {
        std::cout << "Message cannot be empty."
                  << std::endl;

        close(clientSocket);
        return 1;
    }

    // Divide message into small application-level data units
    int totalChunks =
        (message.length() + CHUNK_SIZE - 1) / CHUNK_SIZE;

    std::cout << "\nMessage Size : "
              << message.length() << " bytes"
              << std::endl;

    std::cout << "Data Units   : "
              << totalChunks
              << std::endl;

    std::cout << "\nNetwork Conditions"
              << std::endl;

    std::cout << "Packet Loss       : "
              << packetLoss << "%"
              << std::endl;

    std::cout << "Simulated Latency : "
              << latency << " ms"
              << std::endl;

    std::cout << "\nStarting network test..."
              << std::endl;

    /*
       Decide exactly how many data units should be lost.

       Example:
       10 units with 70% loss
       = 7 units will be lost.
    */
    int lostTarget =
        (totalChunks * packetLoss) / 100;

    // Randomly select which data units will be lost
    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::vector<bool> willBeLost(totalChunks, false);

    int selectedLoss = 0;

    while (selectedLoss < lostTarget) {

        std::uniform_int_distribution<int> distribution(
            0,
            totalChunks - 1
        );

        int index = distribution(generator);

        if (!willBeLost[index]) {

            willBeLost[index] = true;
            selectedLoss++;
        }
    }

    int receivedChunks = 0;
    int lostChunks = 0;

    int receivedBytes = 0;
    int lostBytes = 0;

    double totalLatency = 0.0;

    char buffer[1024];

    // Send each application-level data unit
    for (int i = 0; i < totalChunks; i++) {

        int startPosition = i * CHUNK_SIZE;

        int currentSize = std::min(
            CHUNK_SIZE,
            static_cast<int>(message.length()) - startPosition
        );

        std::string chunk =
            message.substr(startPosition, currentSize);

        std::cout << "\nData Unit "
                  << i + 1
                  << " / "
                  << totalChunks
                  << std::endl;

        std::cout << "Data : "
                  << chunk
                  << std::endl;

        // Apply the selected packet-loss condition
        if (willBeLost[i]) {

            lostChunks++;
            lostBytes += currentSize;

            std::cout << "Status : LOST"
                      << std::endl;

            continue;
        }

        // Start latency measurement
        auto startTime =
            std::chrono::steady_clock::now();

        // Apply simulated latency
        if (latency > 0) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(latency)
            );
        }

        // DATA|chunk number|total chunks|data
        std::string data =
            "DATA|" +
            std::to_string(i + 1) +
            "|" +
            std::to_string(totalChunks) +
            "|" +
            chunk;

        int bytesSent = sendto(
            clientSocket,
            data.c_str(),
            data.length(),
            0,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)
        );

        if (bytesSent < 0) {

            lostChunks++;
            lostBytes += currentSize;

            std::cout << "Status : SEND FAILED"
                      << std::endl;

            continue;
        }

        sockaddr_in responseAddress{};
        socklen_t responseLength =
            sizeof(responseAddress);

        int bytesReceived = recvfrom(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr*)&responseAddress,
            &responseLength
        );

        auto endTime =
            std::chrono::steady_clock::now();

        if (bytesReceived < 0) {

            lostChunks++;
            lostBytes += currentSize;

            std::cout << "Status : No ACK received"
                      << std::endl;

            continue;
        }

        buffer[bytesReceived] = '\0';

        double currentLatency =
            std::chrono::duration<double, std::milli>(
                endTime - startTime
            ).count();

        receivedChunks++;
        receivedBytes += currentSize;
        totalLatency += currentLatency;

        std::cout << "Status  : RECEIVED"
                  << std::endl;

        std::cout << "Latency : "
                  << currentLatency
                  << " ms"
                  << std::endl;
    }

    // Tell the server that the test is complete
    std::string endMessage =
        "END|" +
        std::to_string(totalChunks) +
        "|" +
        std::to_string(message.length());

    sendto(
        clientSocket,
        endMessage.c_str(),
        endMessage.length(),
        0,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    );

    // Calculate final data loss
    double dataLoss = 0.0;

    if (message.length() > 0) {

        dataLoss =
            (lostBytes * 100.0) /
            message.length();
    }

    double averageLatency = 0.0;

    if (receivedChunks > 0) {

        averageLatency =
            totalLatency / receivedChunks;
    }

    std::cout << "\n========== NETWORK TEST RESULT =========="
              << std::endl;

    std::cout << "Original Data     : "
              << message
              << std::endl;

    std::cout << "Data Units        : "
              << totalChunks
              << std::endl;

    std::cout << "Received Units    : "
              << receivedChunks
              << std::endl;

    std::cout << "Lost Units        : "
              << lostChunks
              << std::endl;

    std::cout << "Received Bytes    : "
              << receivedBytes
              << std::endl;

    std::cout << "Lost Bytes        : "
              << lostBytes
              << std::endl;

    std::cout << "Data Loss         : "
              << dataLoss
              << "%"
              << std::endl;

    std::cout << "Average Latency   : "
              << averageLatency
              << " ms"
              << std::endl;

    std::cout << "=========================================="
              << std::endl;

        // Save the latest test result
    std::ofstream resultFile("results/last_test.txt");

    if (resultFile.is_open()) {

        resultFile << "========== LAST NETWORK TEST ==========\n";

        resultFile << "Original Data     : "
                   << message << "\n";

        resultFile << "Packet Loss       : "
                   << packetLoss << "%\n";

        resultFile << "Simulated Latency : "
                   << latency << " ms\n";

        resultFile << "Data Units        : "
                   << totalChunks << "\n";

        resultFile << "Received Units    : "
                   << receivedChunks << "\n";

        resultFile << "Lost Units        : "
                   << lostChunks << "\n";

        resultFile << "Received Bytes    : "
                   << receivedBytes << "\n";

        resultFile << "Lost Bytes        : "
                   << lostBytes << "\n";

        resultFile << "Data Loss         : "
                   << dataLoss << "%\n";

        resultFile << "Average Latency   : "
                   << averageLatency << " ms\n";

        resultFile << "=======================================\n";

        resultFile.close();
    }          

    close(clientSocket);

    return 0;
}
