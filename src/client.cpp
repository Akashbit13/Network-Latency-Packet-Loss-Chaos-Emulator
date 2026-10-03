// iostream is used for input and output operations such as cin and cout.
#include <iostream>

// string is used because our message and data chunks are stored as strings.
#include <string>

// cstdlib provides atoi(), which converts command-line text into an integer.
#include <cstdlib>

// chrono is used to measure how much time each data unit takes.
#include <chrono>

// thread is used for sleep_for(), which creates the simulated network delay.
#include <thread>

// random is used to randomly decide which data units should be lost.
#include <random>

// algorithm is used for std::min(), which helps us calculate the size
// of the last data unit when it contains fewer than CHUNK_SIZE characters.
#include <algorithm>

// fstream is used to save the final test result into a text file.
#include <fstream>

// vector is used to store the loss status of every data unit.
#include <vector>


// These headers provide the basic Linux socket programming functions.
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


// We divide the complete message into small data units of 5 bytes.
// A fixed small size makes it easy to demonstrate data-unit loss.
// We could use a larger value, but 5 makes the loss easy to observe
// during testing.
const int CHUNK_SIZE = 5;


int main(int argc, char* argv[]) {

    /*
       argc = number of command-line arguments.
       argv = actual command-line argument values.

       Our main program starts the client like:

       ./client packetLoss latency

       So we expect exactly 3 arguments:
       argv[0] = program name
       argv[1] = packet loss percentage
       argv[2] = latency in milliseconds

       We use command-line arguments because main.cpp already stores
       the user's network configuration and can directly pass it to
       the client.
    */
    if (argc != 3) {

        // If the required settings are not received, stop the program.
        std::cout << "Invalid network settings." << std::endl;

        return 1;
    }


    /*
       atoi() converts the command-line argument from text to integer.

       For example:
       "30" -> 30
       "100" -> 100

       We use atoi() here because the values are simple integer
       configuration values.
    */
    int packetLoss = std::atoi(argv[1]);

    int latency = std::atoi(argv[2]);


    /*
       Create a UDP socket.

       AF_INET  -> IPv4 communication
       SOCK_DGRAM -> UDP socket

       UDP is selected because this project is about demonstrating
       packet/data-unit loss and latency. UDP does not automatically
       retransmit lost data like TCP does.

       TCP could also be used for networking, but TCP would handle
       reliability and retransmission automatically, which would make
       our application-level loss experiment less clear.
    */
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);


    // socket() returns a negative value when socket creation fails.
    if (clientSocket < 0) {

        // perror() prints the system error message.
        perror("Socket creation failed");

        return 1;
    }


    /*
       Store the server's network address.

       sockaddr_in is a structure used for IPv4 socket addresses.
       It stores information such as IP address and port number.
    */
    sockaddr_in serverAddress{};


    // Tell the socket that we are using IPv4.
    serverAddress.sin_family = AF_INET;


    /*
       The server is listening on port 8080.

       htons() converts the port number into network byte order,
       which is the standard format used by network communication.
    */
    serverAddress.sin_port = htons(8080);


    /*
       INADDR_LOOPBACK represents 127.0.0.1.

       We use localhost because both client and server are running
       on the same computer during our project testing.

       In a real network, this could be replaced by the server's
       actual IP address.
    */
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    // This string will store the complete message entered by the user.
    std::string message;


    // Ask the user for the complete message that will be transmitted.
    std::cout << "\nEnter message to transmit: ";


    /*
       Remove any leftover whitespace/newline from previous input.

       This is useful because getline() reads until the Enter key,
       including spaces inside the message.

       We use getline() instead of cin >> message because the user
       should be able to enter a complete sentence such as:

       "Hello this is my network testing project"
    */
    std::cin >> std::ws;

    std::getline(std::cin, message);


    // Empty messages are not useful for a network transmission test.
    if (message.empty()) {

        std::cout << "Message cannot be empty."
                  << std::endl;


        // Close the socket before leaving the program.
        close(clientSocket);

        return 1;
    }


    /*
       Divide the complete message into small application-level
       data units.

       Example:

       Message = "HelloWorld"
       CHUNK_SIZE = 5

       Data Unit 1 = "Hello"
       Data Unit 2 = "World"

       Formula:

       (length + size - 1) / size

       This formula also correctly handles the last smaller chunk.

       Example:
       12 characters / 5
       -> 3 data units
       -> 5 + 5 + 2
    */
    int totalChunks =
        (message.length() + CHUNK_SIZE - 1) / CHUNK_SIZE;


    // Display the original message size.
    std::cout << "\nMessage Size : "
              << message.length() << " bytes"
              << std::endl;


    // Display how many application-level data units were created.
    std::cout << "Data Units   : "
              << totalChunks
              << std::endl;


    // Display the network conditions selected by the user.
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
       Calculate exactly how many data units should be lost.

       Example:

       Total units = 10
       Packet loss = 70%

       lostTarget = (10 * 70) / 100
                  = 7

       We use data-unit count here instead of simply generating a
       random number for every unit. This makes the configured loss
       percentage predictable.

       Example:
       10 units with 30% loss -> exactly 3 units are selected as lost.
    */
    int lostTarget =
        (totalChunks * packetLoss) / 100;


    /*
       Create a random number generator.

       random_device is used to obtain a random seed.
       mt19937 is a commonly used pseudo-random number generator.

       We need randomness because the lost units should not always be
       the first or last units. Any data unit can be lost.
    */
    std::random_device randomDevice;

    std::mt19937 generator(randomDevice());


    /*
       This vector stores whether each data unit should be lost.

       Example for 5 units:

       false false true false true

       means unit 3 and unit 5 will be lost.

       vector<bool> is used because we only need a yes/no value.
    */
    std::vector<bool> willBeLost(totalChunks, false);


    // Counts how many units have already been selected for loss.
    int selectedLoss = 0;


    /*
       Keep selecting random data units until the required number
       of lost units is reached.

       We check !willBeLost[index] so that the same unit is not
       selected twice.
    */
    while (selectedLoss < lostTarget) {

        /*
           Generate a random data-unit number.

           The range is:
           0 -> totalChunks - 1

           We use 0-based indexing internally because C++ arrays
           and vectors start from index 0.
        */
        std::uniform_int_distribution<int> distribution(
            0,
            totalChunks - 1
        );


        // Select one random data-unit index.
        int index = distribution(generator);


        // Only select the unit if it has not already been selected.
        if (!willBeLost[index]) {

            // Mark this unit as lost.
            willBeLost[index] = true;

            // Increase the number of selected lost units.
            selectedLoss++;
        }
    }


    /*
       These variables keep track of the test results.

       receivedChunks -> number of successfully received units
       lostChunks     -> number of lost/failed units

       receivedBytes  -> total bytes successfully transmitted
       lostBytes      -> total bytes that were lost
    */
    int receivedChunks = 0;

    int lostChunks = 0;

    int receivedBytes = 0;

    int lostBytes = 0;


    // Stores the total time taken by all successfully received units.
    double totalLatency = 0.0;


    /*
       Buffer used to receive the server's ACK response.

       1024 bytes is enough for the small ACK messages used in this
       project.
    */
    char buffer[1024];


    /*
       Send every application-level data unit one by one.

       We use a loop because one complete user message has already
       been divided into multiple smaller units.
    */
    for (int i = 0; i < totalChunks; i++) {


        /*
           Calculate where this data unit starts inside the original
           message.

           Example:

           CHUNK_SIZE = 5

           Unit 1 -> start = 0
           Unit 2 -> start = 5
           Unit 3 -> start = 10
        */
        int startPosition = i * CHUNK_SIZE;


        /*
           Calculate the current data-unit size.

           std::min() is important for the last unit.

           Example:

           Message length = 12
           CHUNK_SIZE = 5

           Units:
           5 bytes
           5 bytes
           2 bytes

           Without min(), the last unit could try to read beyond
           the actual message length.
        */
        int currentSize = std::min(
            CHUNK_SIZE,
            static_cast<int>(message.length()) - startPosition
        );


        /*
           Extract the current data unit from the complete message.

           substr(start, length) returns a part of the original string.
        */
        std::string chunk =
            message.substr(startPosition, currentSize);


        // Show which data unit is currently being processed.
        std::cout << "\nData Unit "
                  << i + 1
                  << " / "
                  << totalChunks
                  << std::endl;


        // Display the actual data inside this unit.
        std::cout << "Data : "
                  << chunk
                  << std::endl;


        /*
           Check whether this unit was selected for packet loss.

           This is our application-level packet-loss simulation.

           If the unit is selected as lost, we do NOT call sendto().
           In other words, the application intentionally drops the
           data unit before sending it to the server.

           Another possible approach would be to send it and then
           intentionally ignore it at the server, but dropping it
           here is simpler for our project.
        */
        if (willBeLost[i]) {

            // Count this unit as lost.
            lostChunks++;

            // Add its size to the lost-byte count.
            lostBytes += currentSize;


            std::cout << "Status : LOST"
                      << std::endl;


            // Move directly to the next data unit.
            continue;
        }


        /*
           Start measuring the time for this data unit.

           steady_clock is used instead of system_clock because
           steady_clock is designed for measuring time intervals.

           System clock can change due to date/time adjustments,
           but steady_clock moves consistently forward.
        */
        auto startTime =
            std::chrono::steady_clock::now();


        /*
           Simulate network latency.

           If latency is 100 ms, the client waits for approximately
           100 ms before sending the data.

           We use sleep_for() because this is a simple application-
           level way to demonstrate delay.

           A real network emulator could use OS-level traffic control,
           but that would be much more complex and is outside the
           scope of this student-level project.
        */
        if (latency > 0) {

            std::this_thread::sleep_for(
                std::chrono::milliseconds(latency)
            );
        }


        /*
           Create a simple application-level protocol.

           Format:

           DATA|chunk number|total chunks|data

           Example:

           DATA|2|5|Hello

           The server can use the '|' separator to understand:
           1. Message type
           2. Data-unit number
           3. Total number of units
           4. Actual data

           We use a simple text format because it is easy to
           understand, debug and demonstrate.
        */
        std::string data =
            "DATA|" +
            std::to_string(i + 1) +
            "|" +
            std::to_string(totalChunks) +
            "|" +
            chunk;


        /*
           sendto() sends the UDP data to the server.

           Parameters:
           clientSocket  -> socket used for communication
           data.c_str()  -> actual data
           data.length() -> data size
           0             -> no special flags
           serverAddress -> destination
           sizeof(...)   -> address structure size

           UDP is connectionless, so we don't need connect() before
           sending every data unit.
        */
        int bytesSent = sendto(
            clientSocket,
            data.c_str(),
            data.length(),
            0,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)
        );


        /*
           sendto() returns a negative value if sending fails.

           In that case, we treat the data unit as lost because the
           server could not receive it.
        */
        if (bytesSent < 0) {

            lostChunks++;

            lostBytes += currentSize;

            std::cout << "Status : SEND FAILED"
                      << std::endl;

            continue;
        }


        /*
           Store the address of the server from which we expect
           the acknowledgement (ACK).
        */
        sockaddr_in responseAddress{};


        // Size of the response address structure.
        socklen_t responseLength =
            sizeof(responseAddress);


        /*
           Wait for the server's ACK.

           The server sends an ACK after successfully receiving
           a data unit.

           This allows the client to know whether the unit reached
           the server successfully.
        */
        int bytesReceived = recvfrom(
            clientSocket,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr*)&responseAddress,
            &responseLength
        );


        /*
           Record the end time after receiving the ACK.

           Therefore, the measured time represents approximately:

           simulated delay
           + UDP send/receive time
           + server processing
           + ACK return time
        */
        auto endTime =
            std::chrono::steady_clock::now();


        /*
           If recvfrom() fails, the client did not receive an ACK.

           We treat that unit as lost/failed for the purpose of
           this network test.
        */
        if (bytesReceived < 0) {

            lostChunks++;

            lostBytes += currentSize;

            std::cout << "Status : No ACK received"
                      << std::endl;

            continue;
        }


        /*
           Add a null character at the end of the received data.

           This converts the received bytes into a proper C-style
           string so that it can safely be treated as text.
        */
        buffer[bytesReceived] = '\0';


        /*
           Calculate the time taken for this data unit.

           duration<double, milli> converts the time difference
           into milliseconds with decimal precision.

           Example:
           100 ms delay may show something like 100.2 ms or 106 ms
           because actual system/network processing also takes time.
        */
        double currentLatency =
            std::chrono::duration<double, std::milli>(
                endTime - startTime
            ).count();


        // This unit was successfully sent and acknowledged.
        receivedChunks++;


        // Add this unit's data size to the received byte count.
        receivedBytes += currentSize;


        // Add this unit's latency to the total latency.
        totalLatency += currentLatency;


        std::cout << "Status  : RECEIVED"
                  << std::endl;


        std::cout << "Latency : "
                  << currentLatency
                  << " ms"
                  << std::endl;
    }


    /*
       Tell the server that all data units have been processed.

       Format:

       END|total chunks|original message length

       Example:

       END|10|46

       The server uses this information to know that the current
       network test is finished and it can calculate which units
       were received and which were missing.
    */
    std::string endMessage =
        "END|" +
        std::to_string(totalChunks) +
        "|" +
        std::to_string(message.length());


    /*
       Send the END message to the server.

       We do not wait for another ACK here because END only tells
       the server that the transmission phase is complete.
    */
    sendto(
        clientSocket,
        endMessage.c_str(),
        endMessage.length(),
        0,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    );


    /*
       Calculate final data loss based on BYTES.

       Formula:

       lost bytes / original message bytes * 100

       This can be slightly different from packet/data-unit loss
       because every data unit does not necessarily contain the
       same number of bytes.

       Example:
       10 units may have 1 lost unit = 10% unit loss,
       but the lost unit might contain fewer bytes, so byte loss
       could be 8% or 10.8%, etc.
    */
    double dataLoss = 0.0;


    // Avoid division by zero if the original message has no data.
    if (message.length() > 0) {

        dataLoss =
            (lostBytes * 100.0) /
            message.length();
    }


    /*
       Average latency tells us the average time taken by the
       successfully received data units.

       We initialize it to 0 because there may be a 100% loss test
       where no unit is received.
    */
    double averageLatency = 0.0;


    // Calculate average only when at least one unit was received.
    if (receivedChunks > 0) {

        averageLatency =
            totalLatency / receivedChunks;
    }


    // Display the final result in the terminal.
    std::cout << "\n========== NETWORK TEST RESULT =========="
              << std::endl;


    // Show the original message entered by the user.
    std::cout << "Original Data     : "
              << message
              << std::endl;


    // Show the total number of application-level data units.
    std::cout << "Data Units        : "
              << totalChunks
              << std::endl;


    // Show how many units were successfully received.
    std::cout << "Received Units    : "
              << receivedChunks
              << std::endl;


    // Show how many units were lost or failed.
    std::cout << "Lost Units        : "
              << lostChunks
              << std::endl;


    // Show the total number of successfully received bytes.
    std::cout << "Received Bytes    : "
              << receivedBytes
              << std::endl;


    // Show the total number of lost bytes.
    std::cout << "Lost Bytes        : "
              << lostBytes
              << std::endl;


    // Show the final byte-based data loss percentage.
    std::cout << "Data Loss         : "
              << dataLoss
              << "%"
              << std::endl;


    // Show the average latency of successfully received units.
    std::cout << "Average Latency   : "
              << averageLatency
              << " ms"
              << std::endl;


    std::cout << "=========================================="
              << std::endl;


    /*
       Save the latest test result into a text file.

       We use ofstream because the project only needs a simple
       local result file. A database would be unnecessary for this
       project and would make the implementation more complicated.
    */
    std::ofstream resultFile("results/last_test.txt");


    // Check whether the result file was opened successfully.
    if (resultFile.is_open()) {


        // Write a heading for the saved result.
        resultFile << "========== LAST NETWORK TEST ==========\n";


        // Save the original message.
        resultFile << "Original Data     : "
                   << message << "\n";


        // Save the configured packet-loss percentage.
        resultFile << "Packet Loss       : "
                   << packetLoss << "%\n";


        // Save the configured simulated latency.
        resultFile << "Simulated Latency : "
                   << latency << " ms\n";


        // Save total number of data units.
        resultFile << "Data Units        : "
                   << totalChunks << "\n";


        // Save successfully received units.
        resultFile << "Received Units    : "
                   << receivedChunks << "\n";


        // Save lost units.
        resultFile << "Lost Units        : "
                   << lostChunks << "\n";


        // Save successfully received bytes.
        resultFile << "Received Bytes    : "
                   << receivedBytes << "\n";


        // Save lost bytes.
        resultFile << "Lost Bytes        : "
                   << lostBytes << "\n";


        // Save calculated byte-based data loss.
        resultFile << "Data Loss         : "
                   << dataLoss << "%\n";


        // Save average latency.
        resultFile << "Average Latency   : "
                   << averageLatency << " ms\n";


        // Write the ending line for better readability.
        resultFile << "=======================================\n";


        /*
           Close the file after writing.

           Closing the file releases the file resource and ensures
           that all written data is properly saved.
        */
        resultFile.close();
    }


    /*
       Close the UDP socket.

       Once the network test is finished, the socket is no longer
       required, so we release the operating-system resource.
    */
    close(clientSocket);


    // return 0 means the program finished successfully.
    return 0;
}