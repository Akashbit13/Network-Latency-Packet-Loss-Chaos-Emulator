#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>


int main() {

    int packetLoss = 0;
    int latency = 0;

    while (true) {

        std::cout << "\n";
        std::cout << "==================================================\n";
        std::cout << "          NETWORK CHAOS EMULATOR\n";
        std::cout << "       Application Resilience Testing Tool\n";
        std::cout << "==================================================\n";
        std::cout << "1. Start New Network Test\n";
        std::cout << "2. Configure Network Conditions\n";
        std::cout << "3. View Current Configuration\n";
        std::cout << "4. View Last Test Results\n";
        std::cout << "5. Reset Test Configuration\n";
        std::cout << "6. Exit\n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        // Start a new network test
        if (choice == 1) {

            std::cout << "\n========== NEW NETWORK TEST ==========\n";

            std::cout << "Current Network Conditions\n";
            std::cout << "Packet Loss       : "
                      << packetLoss << "%\n";

            std::cout << "Simulated Latency : "
                      << latency << " ms\n";

            std::cout << "======================================\n";

            // Pass the current configuration to the client
            std::string command =
                "./client " +
                std::to_string(packetLoss) +
                " " +
                std::to_string(latency);

            system(command.c_str());
        }

        // Configure network conditions
        else if (choice == 2) {

            while (true) {

                std::cout << "\n";
                std::cout << "========== NETWORK CONDITIONS ==========\n";
                std::cout << "1. Configure Packet Loss\n";
                std::cout << "2. Configure Latency\n";
                std::cout << "3. Configure Both\n";
                std::cout << "4. Back\n";
                std::cout << "----------------------------------------\n";
                std::cout << "Enter your choice: ";

                int conditionChoice;
                std::cin >> conditionChoice;

                // Packet loss configuration
                if (conditionChoice == 1 ||
                    conditionChoice == 3) {

                    std::cout << "\n";
                    std::cout << "========== PACKET LOSS ==========\n";
                    std::cout << "1. No Loss (0%)\n";
                    std::cout << "2. Low Loss (10%)\n";
                    std::cout << "3. Moderate Loss (30%)\n";
                    std::cout << "4. High Loss (70%)\n";
                    std::cout << "5. Complete Loss (100%)\n";
                    std::cout << "6. Custom\n";
                    std::cout << "---------------------------------\n";
                    std::cout << "Enter your choice: ";

                    int lossChoice;
                    std::cin >> lossChoice;

                    if (lossChoice == 1) {
                        packetLoss = 0;
                    }
                    else if (lossChoice == 2) {
                        packetLoss = 10;
                    }
                    else if (lossChoice == 3) {
                        packetLoss = 30;
                    }
                    else if (lossChoice == 4) {
                        packetLoss = 70;
                    }
                    else if (lossChoice == 5) {
                        packetLoss = 100;
                    }
                    else if (lossChoice == 6) {

                        std::cout << "Enter packet loss (0-100): ";
                        std::cin >> packetLoss;

                        if (packetLoss < 0 ||
                            packetLoss > 100) {

                            std::cout << "Invalid value.\n";
                            packetLoss = 0;
                        }
                    }
                    else {
                        std::cout << "Invalid choice.\n";
                    }

                    std::cout << "Packet Loss set to "
                              << packetLoss << "%\n";
                }

                // Latency configuration
                if (conditionChoice == 2 ||
                    conditionChoice == 3) {

                    std::cout << "\n";
                    std::cout << "========== LATENCY ==========\n";
                    std::cout << "1. No Delay (0 ms)\n";
                    std::cout << "2. Low Delay (20 ms)\n";
                    std::cout << "3. Moderate Delay (50 ms)\n";
                    std::cout << "4. High Delay (100 ms)\n";
                    std::cout << "5. Extreme Delay (200 ms)\n";
                    std::cout << "6. Custom\n";
                    std::cout << "-----------------------------\n";
                    std::cout << "Enter your choice: ";

                    int latencyChoice;
                    std::cin >> latencyChoice;

                    if (latencyChoice == 1) {
                        latency = 0;
                    }
                    else if (latencyChoice == 2) {
                        latency = 20;
                    }
                    else if (latencyChoice == 3) {
                        latency = 50;
                    }
                    else if (latencyChoice == 4) {
                        latency = 100;
                    }
                    else if (latencyChoice == 5) {
                        latency = 200;
                    }
                    else if (latencyChoice == 6) {

                        std::cout << "Enter latency in ms: ";
                        std::cin >> latency;

                        if (latency < 0) {

                            std::cout << "Invalid value.\n";
                            latency = 0;
                        }
                    }
                    else {
                        std::cout << "Invalid choice.\n";
                    }

                    std::cout << "Latency set to "
                              << latency << " ms\n";
                }

                // Only Back returns to main menu
                if (conditionChoice == 4) {
                    break;
                }

                if (conditionChoice < 1 ||
                    conditionChoice > 4) {

                    std::cout << "Invalid choice.\n";
                }
            }
        }

        // View current configuration
        else if (choice == 3) {

            std::cout << "\n========== CURRENT CONFIGURATION ==========\n";

            std::cout << "Packet Loss       : "
                      << packetLoss << "%\n";

            std::cout << "Simulated Latency : "
                      << latency << " ms\n";

            std::cout << "===========================================\n";
        }

        // Last test result
        else if (choice == 4) {

            std::ifstream resultFile("results/last_test.txt");

            if (resultFile.is_open()) {

                std::string line;

                std::cout << "\n";

                while (std::getline(resultFile, line)) {
                    std::cout << line << std::endl;
                }

                resultFile.close();
            }
            else {

                std::cout << "\n========== LAST TEST RESULTS ==========\n";
                std::cout << "No test has been completed yet.\n";
                std::cout << "=======================================\n";
            }
        }

        // Reset configuration
        else if (choice == 5) {

            packetLoss = 0;
            latency = 0;

            std::cout << "\nNetwork configuration reset.\n";
            std::cout << "Packet Loss       : 0%\n";
            std::cout << "Simulated Latency : 0 ms\n";
        }

        // Exit
        else if (choice == 6) {

            std::cout << "\nExiting Network Chaos Emulator...\n";
            break;
        }

        else {
            std::cout << "\nInvalid choice. Please try again.\n";
        }
    }

    return 0;
}
