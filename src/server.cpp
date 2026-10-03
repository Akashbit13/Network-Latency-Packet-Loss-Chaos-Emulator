// iostream is used for displaying server status and test results
// on the terminal using cout.
#include <iostream>

// string is used to store received data units and protocol messages.
#include <string>

// cstdio provides perror(), which displays system-level error messages.
#include <cstdio>


// These headers provide Linux socket programming functions.
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


/*
   Maximum number of data units that the server can store for one test.

   We use a fixed value of 100 because our project only needs a
   simple and predictable storage limit.

   A dynamic container such as vector could also be used, but a
   fixed array keeps this student-level implementation simple.
*/
const int MAX_CHUNKS = 100;


int main() {

    /*
       Create a UDP socket.

       AF_INET:
       -> Use IPv4.

       SOCK_DGRAM:
       -> Use UDP.

       UDP is used because our project is demonstrating application-
       level data-unit loss and latency.

       TCP could also be used, but TCP automatically provides
       retransmission and reliable delivery, which is not required
       for our basic chaos-emulation experiment.
    */
    int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);


    /*
       socket() returns a negative value when socket creation fails.

       If that happens, there is no point continuing because the
       server cannot communicate without a socket.
    */
    if (serverSocket < 0) {

        // perror() prints the reason provided by the operating system.
        perror("Socket creation failed");

        return 1;
    }


    /*
       Create and configure the server's IPv4 address.

       sockaddr_in is the standard structure used for IPv4
       socket addresses.
    */
    sockaddr_in serverAddress{};


    // Tell the socket that this is an IPv4 address.
    serverAddress.sin_family = AF_INET;


    /*
       The server listens on port 8080.

       htons() converts the port number into network byte order,
       which is required for network communication.
    */
    serverAddress.sin_port = htons(8080);


    /*
       INADDR_ANY means the server can accept UDP packets sent
       to this machine on the selected port.

       We use this instead of 127.0.0.1 on the server so that the
       server is not restricted to one specific local interface.

       The client still sends to 127.0.0.1 because both programs
       are running on the same machine.
    */
    serverAddress.sin_addr.s_addr = INADDR_ANY;


    /*
       Bind the socket to port 8080.

       bind() tells the operating system:

       "Deliver UDP packets arriving on port 8080 to this socket."

       Without bind(), the server would not know which port it
       should listen on.
    */
    if (bind(
            serverSocket,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress)) < 0) {


        // Display the reason if binding fails.
        perror("Bind failed");


        // Close the socket because the server cannot continue.
        close(serverSocket);

        return 1;
    }


    /*
       At this point the socket has been successfully created
       and bound to port 8080, so the server is ready to receive
       data from the client.
    */
    std::cout << "UDP Server started on port 8080."
              << std::endl;

    std::cout << "Waiting for network test..."
              << std::endl;


    /*
       Buffer used to temporarily store incoming UDP data.

       1024 bytes is more than enough for the small DATA and ACK
       messages used in this project.
    */
    char buffer[1024];


    /*
       This structure stores information about the client that
       sent the current UDP packet.

       recvfrom() fills this structure automatically.
    */
    sockaddr_in clientAddress{};


    // Store the size of the client address structure.
    socklen_t clientLength = sizeof(clientAddress);


    /*
       Store the actual data received for each data unit.

       Example:

       receivedChunks[0] -> first data unit
       receivedChunks[1] -> second data unit
       receivedChunks[2] -> third data unit

       We use an array because the maximum number of units is
       already limited by MAX_CHUNKS.
    */
    std::string receivedChunks[MAX_CHUNKS];


    /*
       This array tells us whether a particular data unit was
       successfully received.

       false -> unit not received
       true  -> unit received

       The array is initialized with false values using {}.
    */
    bool chunkReceived[MAX_CHUNKS] = {};


    /*
       Store information about the current network test.

       totalChunks:
       Total number of units expected.

       receivedChunksCount:
       Number of units successfully received.

       receivedBytes:
       Total number of bytes received successfully.
    */
    int totalChunks = 0;

    int receivedChunksCount = 0;

    int receivedBytes = 0;


    /*
       Keep the server running continuously.

       This is useful because the user can perform multiple tests
       from main.cpp without restarting the server every time.

       The server only stops if an error occurs or the program
       is manually terminated.
    */
    while (true) {


        /*
           Wait for a UDP packet from the client.

           recvfrom() is a blocking call, which means the server
           waits here until some UDP data arrives.

           This is suitable for our simple project because we only
           need to handle one test at a time.
        */
        int bytesReceived = recvfrom(
            serverSocket,
            buffer,
            sizeof(buffer) - 1,
            0,
            (struct sockaddr*)&clientAddress,
            &clientLength
        );


        /*
           A negative value means receiving the data failed.

           Since the server cannot continue normally in this case,
           we leave the receiving loop.
        */
        if (bytesReceived < 0) {

            perror("Receiving data failed");

            break;
        }


        /*
           Add a null character at the end of the received bytes.

           This allows us to treat the buffer as a normal C-style
           string.
        */
        buffer[bytesReceived] = '\0';


        /*
           Convert the received character buffer into a C++ string.

           Using std::string makes operations such as find(),
           substr() and comparison easier.
        */
        std::string data(buffer);


        /*
           Our client sends DATA messages in this format:

           DATA|chunk number|total chunks|data

           Example:

           DATA|2|5|Hello

           The server checks whether the received message starts
           with "DATA|".
        */
        if (data.rfind("DATA|", 0) == 0) {


            /*
               Find the first separator after "DATA|".

               Example:

               DATA|2|5|Hello
                    ^
                    first separator

               We start searching from position 5 because
               "DATA|" contains 5 characters.
            */
            size_t firstSeparator =
                data.find('|', 5);


            /*
               Find the second separator.

               This separates the total number of chunks from
               the actual data.
            */
            size_t secondSeparator =
                data.find('|', firstSeparator + 1);


            /*
               If either separator is missing, the message does not
               follow our expected protocol format.

               We simply ignore that invalid message and wait for
               the next packet.
            */
            if (firstSeparator == std::string::npos ||
                secondSeparator == std::string::npos) {

                continue;
            }


            /*
               Extract the data-unit number.

               Example:

               DATA|2|5|Hello

               data.substr(5, firstSeparator - 5)
               gives "2".

               stoi() converts "2" into integer 2.
            */
            int chunkNumber = std::stoi(
                data.substr(
                    5,
                    firstSeparator - 5
                )
            );


            /*
               Extract the total number of data units.

               Example:

               DATA|2|5|Hello

               This gives 5.
            */
            totalChunks = std::stoi(
                data.substr(
                    firstSeparator + 1,
                    secondSeparator - firstSeparator - 1
                )
            );


            /*
               Extract the actual data unit.

               Everything after the second '|'
               is considered the actual message data.

               Example:

               DATA|2|5|Hello

               chunk = "Hello"
            */
            std::string chunk =
                data.substr(secondSeparator + 1);


            /*
               Convert the data-unit number into a C++ array index.

               Data units are numbered from 1:

               Unit 1
               Unit 2
               Unit 3

               But arrays start from index 0:

               index 0
               index 1
               index 2

               Therefore we subtract 1.
            */
            int index = chunkNumber - 1;


            /*
               Make sure the received unit number is inside
               the allowed array range.

               This prevents accessing memory outside the array.
            */
            if (index >= 0 && index < MAX_CHUNKS) {


                /*
                   Check whether this unit was already received.

                   UDP does not guarantee delivery or ordering, and
                   duplicate packets are possible in real networks.

                   We therefore avoid counting the same unit twice.
                */
                if (!chunkReceived[index]) {


                    // Store the received data unit in its position.
                    receivedChunks[index] = chunk;


                    // Mark this unit as successfully received.
                    chunkReceived[index] = true;


                    // Increase the total number of received units.
                    receivedChunksCount++;


                    // Add this unit's size to the received byte count.
                    receivedBytes += chunk.length();


                    // Display the received unit on the server.
                    std::cout << "\nData Unit "
                              << chunkNumber
                              << " received: "
                              << chunk
                              << std::endl;
                }
            }


            /*
               Send an acknowledgement (ACK) back to the client.

               The ACK tells the client:

               "I received this data unit."

               We use ACK because the client needs a simple way
               to confirm successful delivery.

               TCP normally handles acknowledgement internally,
               but because we are using UDP, we implement a very
               simple ACK at the application level.
            */
            std::string acknowledgement =
                "ACK|" +
                std::to_string(chunkNumber);


            /*
               Send the ACK back to the same client that sent
               the DATA packet.

               sendto() is used because UDP is connectionless.
            */
            sendto(
                serverSocket,
                acknowledgement.c_str(),
                acknowledgement.length(),
                0,
                (struct sockaddr*)&clientAddress,
                clientLength
            );
        }


        /*
           END message means the client has finished sending all
           data units for the current test.

           Format:

           END|total chunks|original message length

           The server can now calculate which units were received
           and which ones were lost.
        */
        else if (data.rfind("END|", 0) == 0) {


            std::cout << "\n";


            std::cout << "========== SERVER TEST RESULT =========="
                      << std::endl;


            /*
               Display every expected data unit.

               This is useful for demonstrating packet/data-unit loss.

               Example:

               Data Unit 1 : RECEIVED -> Hello
               Data Unit 2 : LOST
               Data Unit 3 : RECEIVED -> World
            */
            for (int i = 0;
                 i < totalChunks && i < MAX_CHUNKS;
                 i++) {


                /*
                   If chunkReceived[i] is true, that data unit
                   successfully reached the server.
                */
                if (chunkReceived[i]) {

                    std::cout << "Data Unit "
                              << i + 1
                              << " : RECEIVED -> "
                              << receivedChunks[i]
                              << std::endl;
                }


                /*
                   If the value is false, the server never received
                   this expected data unit.

                   In our project, this normally happens because
                   client.cpp intentionally dropped the unit.
                */
                else {

                    std::cout << "Data Unit "
                              << i + 1
                              << " : LOST"
                              << std::endl;
                }
            }


            /*
               Calculate the number of lost data units.

               Example:

               Total units = 10
               Received    = 7

               Lost = 10 - 7 = 3
            */
            int lostChunks =
                totalChunks - receivedChunksCount;


            // Display the number of successfully received units.
            std::cout << "\nReceived Units : "
                      << receivedChunksCount
                      << std::endl;


            // Display the number of missing/lost units.
            std::cout << "Lost Units     : "
                      << lostChunks
                      << std::endl;


            // Display the total bytes successfully received.
            std::cout << "Received Bytes : "
                      << receivedBytes
                      << std::endl;


            /*
               Reconstruct the received message.

               We go through the units in their original order.

               If a unit exists, print its data.
               If it is missing, print [LOST].

               This makes the effect of packet/data-unit loss
               visually easy to understand.
            */
            std::cout << "\nReceived Data  : ";


            for (int i = 0;
                 i < totalChunks && i < MAX_CHUNKS;
                 i++) {


                // Print the actual data when the unit was received.
                if (chunkReceived[i]) {

                    std::cout << receivedChunks[i];
                }


                /*
                   If the unit was not received, we cannot recover
                   its original data because UDP does not provide
                   automatic retransmission.

                   So we display [LOST] as a clear placeholder.
                */
                else {

                    std::cout << "[LOST]";
                }
            }


            std::cout << std::endl;


            std::cout << "========================================"
                      << std::endl;


            /*
               Clear the stored data before the next network test.

               This is important because the server remains running
               and the user can start another test.

               Without clearing these arrays, old test data could
               appear in the next test.
            */
            for (int i = 0; i < MAX_CHUNKS; i++) {

                // Remove the previous data stored at this position.
                receivedChunks[i].clear();


                // Mark the unit as not received for the next test.
                chunkReceived[i] = false;
            }


            /*
               Reset the counters for the next test.
            */
            totalChunks = 0;

            receivedChunksCount = 0;

            receivedBytes = 0;
        }
    }


    /*
       Close the UDP socket before the program terminates.

       This releases the socket resource allocated by the operating
       system.
    */
    close(serverSocket);


    /*
       return 0 means the server program ended successfully.

       Normally the server stays inside the while loop, so this line
       is reached only when the receiving loop is stopped.
    */
    return 0;
}