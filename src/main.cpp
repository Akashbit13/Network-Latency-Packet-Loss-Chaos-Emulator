// iostream is used for console input and output.
// We use cin to take choices from the user and cout to display menus/results.
#include <iostream>

// cstdlib provides the system() function.
// We use system() to start the client program from this controller.
#include <cstdlib>

// string is used for creating the command that starts the client
// with packet loss and latency values.
#include <string>

// fstream is used to read the saved result file
// from results/last_test.txt.
#include <fstream>


int main() {

    /*
       These two variables store the current network configuration.

       packetLoss -> percentage of application-level data units to lose.
       latency    -> simulated delay in milliseconds.

       They start from 0, meaning:
       No packet/data-unit loss
       No simulated delay

       We keep these values in main.cpp because this file works as
       the main controller of the project.
    */
    int packetLoss = 0;
    int latency = 0;


    /*
       Keep the main menu running until the user selects Exit.

       A while(true) loop is used because the user should be able
       to perform multiple network tests without restarting the program.

       An alternative would be a do-while loop, but while(true) with
       break makes the menu flow easier to control when there are
       multiple menu options.
    */
    while (true) {

        // Print an empty line to keep the menu readable.
        std::cout << "\n";


        // Main title of the application.
        std::cout << "==================================================\n";
        std::cout << "          NETWORK CHAOS EMULATOR\n";
        std::cout << "       Application Resilience Testing Tool\n";
        std::cout << "==================================================\n";


        /*
           Main menu.

           We keep only the important operations here:
           1. Start a test
           2. Configure network conditions
           3. View current configuration
           4. View previous result
           5. Reset configuration
           6. Exit

           More advanced options are avoided because this is a
           command-line based student project.
        */
        std::cout << "1. Start New Network Test\n";
        std::cout << "2. Configure Network Conditions\n";
        std::cout << "3. View Current Configuration\n";
        std::cout << "4. View Last Test Results\n";
        std::cout << "5. Reset Test Configuration\n";
        std::cout << "6. Exit\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "Enter your choice: ";


        // Store the option selected by the user.
        int choice;

        std::cin >> choice;


        /*
           OPTION 1:
           Start a new network test.

           main.cpp does not perform the actual UDP communication.
           Instead, it controls the configuration and starts client.cpp.

           This separation keeps the project simple:
           main.cpp -> menu/configuration
           client.cpp -> transmission and measurement
           server.cpp -> receiving and ACK
        */
        if (choice == 1) {

            std::cout << "\n========== NEW NETWORK TEST ==========\n";


            // Show the configuration that will be used for this test.
            std::cout << "Current Network Conditions\n";


            std::cout << "Packet Loss       : "
                      << packetLoss << "%\n";


            std::cout << "Simulated Latency : "
                      << latency << " ms\n";


            std::cout << "======================================\n";


            /*
               Build the command used to start client.cpp.

               Example:

               packetLoss = 30
               latency = 100

               Command becomes:

               ./client 30 100

               client.cpp receives these values through argc/argv.

               We use command-line arguments because it avoids creating
               shared global variables between main.cpp and client.cpp.
               It also keeps client.cpp independently executable.
            */
            std::string command =
                "./client " +
                std::to_string(packetLoss) +
                " " +
                std::to_string(latency);


            /*
               system() executes the command through the operating system.

               Here it starts the compiled client program.

               Another approach would be to combine all client logic
               directly into main.cpp, but that would make the main
               controller very large. Keeping client.cpp separate
               makes the project easier to understand.
            */
            system(command.c_str());
        }


        /*
           OPTION 2:
           Configure network conditions.

           A second while loop is used because the configuration menu
           should remain open while the user changes settings.

           Only selecting "Back" should return to the main menu.
        */
        else if (choice == 2) {

            while (true) {

                // Display the network-condition submenu.
                std::cout << "\n";
                std::cout << "========== NETWORK CONDITIONS ==========\n";
                std::cout << "1. Configure Packet Loss\n";
                std::cout << "2. Configure Latency\n";
                std::cout << "3. Configure Both\n";
                std::cout << "4. Back\n";
                std::cout << "----------------------------------------\n";
                std::cout << "Enter your choice: ";


                // Store the selected configuration option.
                int conditionChoice;

                std::cin >> conditionChoice;


                /*
                   Packet loss configuration.

                   This block runs when:
                   1 -> user wants only packet loss
                   3 -> user wants both packet loss and latency

                   Using the same block for both options avoids
                   duplicating the packet-loss code.
                */
                if (conditionChoice == 1 ||
                    conditionChoice == 3) {

                    std::cout << "\n";
                    std::cout << "========== PACKET LOSS ==========\n";


                    /*
                       Predefined packet-loss levels.

                       These values make testing quick for a user who
                       does not want to enter a custom percentage.

                       Custom option is still provided for flexibility.
                    */
                    std::cout << "1. No Loss (0%)\n";
                    std::cout << "2. Low Loss (10%)\n";
                    std::cout << "3. Moderate Loss (30%)\n";
                    std::cout << "4. High Loss (70%)\n";
                    std::cout << "5. Complete Loss (100%)\n";
                    std::cout << "6. Custom\n";
                    std::cout << "---------------------------------\n";
                    std::cout << "Enter your choice: ";


                    // Store the packet-loss menu choice.
                    int lossChoice;

                    std::cin >> lossChoice;


                    /*
                       Convert the selected menu option into an
                       actual packet-loss percentage.
                    */
                    if (lossChoice == 1) {

                        // No data units will be intentionally lost.
                        packetLoss = 0;
                    }

                    else if (lossChoice == 2) {

                        // 10% of the application-level data units.
                        packetLoss = 10;
                    }

                    else if (lossChoice == 3) {

                        // 30% data-unit loss.
                        packetLoss = 30;
                    }

                    else if (lossChoice == 4) {

                        // 70% data-unit loss.
                        packetLoss = 70;
                    }

                    else if (lossChoice == 5) {

                        // 100% means every data unit is selected for loss.
                        packetLoss = 100;
                    }

                    else if (lossChoice == 6) {

                        /*
                           Custom value allows the user to enter any
                           percentage from 0 to 100.
                        */
                        std::cout << "Enter packet loss (0-100): ";

                        std::cin >> packetLoss;


                        /*
                           Packet loss cannot be below 0% or above 100%.

                           If an invalid value is entered, we reset it
                           to 0 instead of keeping an invalid configuration.
                        */
                        if (packetLoss < 0 ||
                            packetLoss > 100) {

                            std::cout << "Invalid value.\n";

                            packetLoss = 0;
                        }
                    }

                    else {

                        // Handle an invalid menu option.
                        std::cout << "Invalid choice.\n";
                    }


                    // Show the currently selected packet-loss value.
                    std::cout << "Packet Loss set to "
                              << packetLoss << "%\n";
                }


                /*
                   Latency configuration.

                   This block runs when:
                   2 -> user wants only latency
                   3 -> user wants both loss and latency

                   Like packet loss, predefined values make testing
                   quick and easy.
                */
                if (conditionChoice == 2 ||
                    conditionChoice == 3) {

                    std::cout << "\n";
                    std::cout << "========== LATENCY ==========\n";


                    // Display predefined latency options.
                    std::cout << "1. No Delay (0 ms)\n";
                    std::cout << "2. Low Delay (20 ms)\n";
                    std::cout << "3. Moderate Delay (50 ms)\n";
                    std::cout << "4. High Delay (100 ms)\n";
                    std::cout << "5. Extreme Delay (200 ms)\n";
                    std::cout << "6. Custom\n";
                    std::cout << "-----------------------------\n";
                    std::cout << "Enter your choice: ";


                    // Store the selected latency option.
                    int latencyChoice;

                    std::cin >> latencyChoice;


                    /*
                       Convert the selected menu option into the
                       actual latency value in milliseconds.
                    */
                    if (latencyChoice == 1) {

                        // No simulated delay.
                        latency = 0;
                    }

                    else if (latencyChoice == 2) {

                        // Simulate 20 milliseconds of delay.
                        latency = 20;
                    }

                    else if (latencyChoice == 3) {

                        // Simulate 50 milliseconds of delay.
                        latency = 50;
                    }

                    else if (latencyChoice == 4) {

                        // Simulate 100 milliseconds of delay.
                        latency = 100;
                    }

                    else if (latencyChoice == 5) {

                        // Simulate 200 milliseconds of delay.
                        latency = 200;
                    }

                    else if (latencyChoice == 6) {

                        /*
                           Allow the user to enter a custom latency.

                           Example:
                           150 -> 150 milliseconds
                        */
                        std::cout << "Enter latency in ms: ";

                        std::cin >> latency;


                        /*
                           Negative latency does not make sense.

                           Therefore, any negative value is treated
                           as invalid and reset to 0.
                        */
                        if (latency < 0) {

                            std::cout << "Invalid value.\n";

                            latency = 0;
                        }
                    }

                    else {

                        // Handle invalid latency menu choices.
                        std::cout << "Invalid choice.\n";
                    }


                    // Display the selected latency.
                    std::cout << "Latency set to "
                              << latency << " ms\n";
                }


                /*
                   OPTION 4:
                   Go back to the main menu.

                   break exits only this inner configuration loop.
                   The outer main menu loop continues running.
                */
                if (conditionChoice == 4) {

                    break;
                }


                /*
                   Check for an invalid configuration-menu option.

                   Valid choices are 1, 2, 3 and 4.
                */
                if (conditionChoice < 1 ||
                    conditionChoice > 4) {

                    std::cout << "Invalid choice.\n";
                }
            }
        }


        /*
           OPTION 3:
           Display the current configuration.

           This does not change anything. It only allows the user
           to verify the settings before starting a test.
        */
        else if (choice == 3) {

            std::cout << "\n========== CURRENT CONFIGURATION ==========\n";


            // Display current packet-loss configuration.
            std::cout << "Packet Loss       : "
                      << packetLoss << "%\n";


            // Display current latency configuration.
            std::cout << "Simulated Latency : "
                      << latency << " ms\n";


            std::cout << "===========================================\n";
        }


        /*
           OPTION 4:
           Display the result of the most recent network test.

           client.cpp saves the result in:
           results/last_test.txt

           main.cpp simply reads and displays that file.

           A database was not used because we only need the latest
           test result. A simple text file is enough for this project.
        */
        else if (choice == 4) {

            /*
               Open the saved result file in read mode.

               ifstream is used because we only need to read data.
            */
            std::ifstream resultFile("results/last_test.txt");


            // Check whether the result file exists and opened correctly.
            if (resultFile.is_open()) {

                // Store one line of the result at a time.
                std::string line;


                std::cout << "\n";


                /*
                   Read the file line by line until the end.

                   getline() is used because the result contains
                   complete lines with spaces.
                */
                while (std::getline(resultFile, line)) {

                    // Display the current line on the terminal.
                    std::cout << line << std::endl;
                }


                // Close the file after reading.
                resultFile.close();
            }

            else {

                /*
                   If the file does not exist, it means the user has
                   not completed any network test yet.
                */
                std::cout << "\n========== LAST TEST RESULTS ==========\n";
                std::cout << "No test has been completed yet.\n";
                std::cout << "=======================================\n";
            }
        }


        /*
           OPTION 5:
           Reset the network configuration.

           Both values are returned to their default state:

           Packet Loss = 0%
           Latency     = 0 ms

           This gives the user an easy way to start a fresh test.
        */
        else if (choice == 5) {

            // Reset packet loss.
            packetLoss = 0;


            // Reset simulated latency.
            latency = 0;


            // Confirm the reset operation to the user.
            std::cout << "\nNetwork configuration reset.\n";

            std::cout << "Packet Loss       : 0%\n";

            std::cout << "Simulated Latency : 0 ms\n";
        }


        /*
           OPTION 6:
           Exit the program.

           break stops the main while(true) loop.
        */
        else if (choice == 6) {

            std::cout << "\nExiting Network Chaos Emulator...\n";


            // Exit the main menu loop.
            break;
        }


        /*
           If the user enters anything other than 1-6,
           show an error and display the main menu again.
        */
        else {

            std::cout << "\nInvalid choice. Please try again.\n";
        }
    }


    /*
       return 0 means the program completed normally.

       The operating system receives this value as the program's
       successful exit status.
    */
    return 0;
}