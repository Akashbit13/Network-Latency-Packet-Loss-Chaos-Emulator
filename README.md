# Network Latency & Packet-Loss Chaos Emulator

### Application Resilience Testing Tool

A C++ and UDP-based application-level network chaos emulator designed to demonstrate how an application behaves under controlled network conditions such as data loss and latency.

The project allows a user to enter a complete message, divide it into smaller application-level data units, transmit those units using UDP, intentionally simulate data-unit loss, introduce configurable latency, and measure the resulting network behavior.

The system displays received units, lost units, received bytes, lost bytes, data loss percentage, and average latency.

---

## 📌 Project Overview

In real-world network communication, applications may experience different network conditions such as:

- Packet or data loss
- Network latency
- Delayed responses
- Missing data
- Unreliable communication

Applications need to be designed and tested so that they can handle such conditions properly.

The **Network Latency & Packet-Loss Chaos Emulator** provides a simple command-line environment to simulate these conditions at the application level.

The project uses a UDP client-server architecture. A complete user-entered message is divided into smaller data units. Based on the selected network configuration, some data units can be intentionally dropped and a configurable delay can be introduced before transmission.

The received data and network measurements are then displayed to the user and the latest test result is stored locally.

---

# 🎯 Objectives

The main objectives of this project are:

1. To understand basic UDP socket communication.
2. To implement a simple client-server communication model.
3. To divide application data into smaller transmission units.
4. To simulate controlled application-level data loss.
5. To simulate configurable network latency.
6. To track received and lost data units.
7. To calculate received and lost bytes.
8. To calculate the overall data loss percentage.
9. To measure average transmission latency.
10. To understand the effect of unreliable network conditions on data transmission.
11. To practice C++ programming in a Linux environment.
12. To understand basic network resilience testing concepts.

---

# ✨ Key Features

- Command-line based user interface
- UDP client-server communication
- Application-level data-unit transmission
- Configurable packet/data-unit loss
- Configurable latency simulation
- Predefined packet-loss levels
- Custom packet-loss percentage
- Predefined latency levels
- Custom latency value
- Application-level ACK mechanism
- Data-unit tracking
- Lost-unit detection
- Received/lost byte calculation
- Average latency calculation
- Server-side data reconstruction
- Last test result storage
- Multiple network tests without restarting the server
- Simple and easy-to-understand implementation

---

# 🏗️ System Architecture

The project is divided into three main C++ components:

```text
                    +-----------------------------+
                    |          main.cpp           |
                    |-----------------------------|
                    | Main Menu                   |
                    | Network Configuration      |
                    | Test Controller             |
                    | Result Viewer                |
                    +-------------+---------------+
                                  |
                                  |
                                  | Starts client
                                  v
                    +-----------------------------+
                    |         client.cpp          |
                    |-----------------------------|
                    | Message Input               |
                    | Data Unit Creation          |
                    | Loss Simulation             |
                    | Latency Simulation          |
                    | UDP Transmission            |
                    | ACK Handling                |
                    | Latency Measurement         |
                    | Result Calculation          |
                    +-------------+---------------+
                                  |
                                  |
                                  | UDP DATA
                                  v
                    +-----------------------------+
                    |         server.cpp          |
                    |-----------------------------|
                    | UDP Receiver                |
                    | Data Unit Storage            |
                    | ACK Generation               |
                    | Missing Unit Detection      |
                    | Data Reconstruction         |
                    +-----------------------------+
```

---

# 🔄 Project Workflow

The complete workflow of the application is:

```text
       Start Application
              |
              v
   Configure Network Conditions
              |
              v
      Start New Network Test
              |
              v
        Enter Full Message
              |
              v
      Divide Message into
       Data Units / Chunks
              |
              v
       Apply Packet Loss
       and Simulated Latency
              |
              v
       Send Data Using UDP
              |
              v
        Server Receives
          Available Units
              |
              v
          Server Sends ACK
              |
              v
       Client Measures
      Transmission Results
              |
              v
       Display Test Result
              |
              v
      Save Latest Test Result
```

---

# 🖥️ Main Menu

The application provides the following main menu:

```text
==================================================
          NETWORK CHAOS EMULATOR
       Application Resilience Testing Tool
==================================================
1. Start New Network Test
2. Configure Network Conditions
3. View Current Configuration
4. View Last Test Results
5. Reset Test Configuration
6. Exit
--------------------------------------------------
Enter your choice:
```

### Menu Description

| Option | Description |
|---|---|
| 1 | Starts a new network test |
| 2 | Configures packet loss and latency |
| 3 | Displays the current network configuration |
| 4 | Displays the latest saved test result |
| 5 | Resets network conditions to default |
| 6 | Exits the application |

---

# ⚙️ Network Configuration

The project allows the user to configure two main network conditions:

1. Packet/Data-Unit Loss
2. Simulated Latency

---

## 📉 Packet Loss Configuration

Available options:

```text
1. No Loss (0%)
2. Low Loss (10%)
3. Moderate Loss (30%)
4. High Loss (70%)
5. Complete Loss (100%)
6. Custom
```

The user can also enter a custom value between:

```text
0% - 100%
```

### Example

If there are 10 data units and the selected loss is 30%:

```text
Total Data Units = 10

Expected Lost Units = 3
```

The application randomly selects which data units will be lost.

---

# ⏱️ Latency Configuration

Available options:

```text
1. No Delay (0 ms)
2. Low Delay (20 ms)
3. Moderate Delay (50 ms)
4. High Delay (100 ms)
5. Extreme Delay (200 ms)
6. Custom
```

The user can also enter a custom latency value in milliseconds.

### Example

```text
Configured Latency = 100 ms
```

The client introduces approximately 100 milliseconds of delay before sending a data unit.

The measured latency can be slightly higher because the measurement also includes:

- Local processing
- UDP transmission
- Server processing
- ACK response
- Operating system overhead

---

# 📦 Data Unit Concept

The project does not ask the user to manually enter individual packets.

Instead, the user enters one complete message.

Example:

```text
hello akash this is my network testing project
```

The client automatically divides the message into smaller application-level data units.

The current data-unit size is:

```text
5 bytes
```

For example:

```text
Original Message:

hello akash this is my network testing project

        |
        v

+-------+-------+-------+-------+-------+
| Unit1 | Unit2 | Unit3 | Unit4 | Unit5 |
+-------+-------+-------+-------+-------+
```

The last data unit can contain fewer than 5 bytes if the message length is not divisible by 5.

---

# 📡 UDP Communication

The project uses **UDP (User Datagram Protocol)** for communication.

The server listens on:

```text
Port: 8080
```

During local testing, the client communicates with:

```text
127.0.0.1
```

which is the localhost/loopback address.

---

# ❓ Why UDP?

UDP was selected because it provides a simple datagram-based communication model.

It does not automatically provide the same reliable delivery and retransmission mechanism that TCP provides.

This makes it easier to demonstrate application-level data loss.

### Why not TCP?

TCP provides reliable communication and automatically handles retransmission and ordering.

For this particular demonstration, using UDP makes the intentionally lost data units easier to observe.

TCP can be considered as a possible future comparison mode.

---

# 🧩 Application-Level Packet/Data Loss

The project simulates loss at the application level.

Suppose the message is divided into:

```text
Unit 1
Unit 2
Unit 3
Unit 4
Unit 5
```

If Unit 3 is selected as lost:

```text
Unit 1 → Sent
Unit 2 → Sent
Unit 3 → LOST
Unit 4 → Sent
Unit 5 → Sent
```

The client intentionally does not send the selected lost data unit to the server.

This provides a simple way to demonstrate how missing application data can affect communication.

---

# 📡 Data Transmission Format

A simple text-based application protocol is used.

This keeps the protocol easy to understand and debug.

---

## DATA Message

The format is:

```text
DATA|chunk_number|total_chunks|data
```

Example:

```text
DATA|2|5|Hello
```

Meaning:

```text
DATA       → Message type
2          → Current data-unit number
5          → Total number of data units
Hello      → Actual data
```

---

# ✅ ACK Message

After receiving a DATA message, the server sends an acknowledgement.

Format:

```text
ACK|chunk_number
```

Example:

```text
ACK|2
```

This tells the client that data unit 2 was received by the server.

The ACK is implemented at the application level because UDP itself does not provide application-level confirmation for a specific data unit.

---

# 🏁 END Message

After all data units have been processed, the client sends an END message.

Format:

```text
END|total_chunks|message_length
```

Example:

```text
END|10|46
```

This tells the server that the current test has finished.

The server then checks all expected data units and identifies which ones were received and which ones were missing.

---

# 🖥️ Server-Side Processing

The server performs the following operations:

1. Creates a UDP socket.
2. Binds the socket to port 8080.
3. Waits for incoming DATA messages.
4. Extracts the data-unit number.
5. Extracts the total number of data units.
6. Extracts the actual data.
7. Stores the received data unit.
8. Sends an ACK to the client.
9. Waits for the END message.
10. Checks all expected data units.
11. Identifies lost units.
12. Reconstructs the received message.

---

# 📊 Test Result Metrics

The project displays the following measurements.

### Original Data

The complete message entered by the user.

### Data Units

Total number of application-level data units created from the message.

### Received Units

Number of data units successfully received by the server.

### Lost Units

Number of data units that were intentionally dropped or failed during transmission.

### Received Bytes

Total number of bytes successfully received.

### Lost Bytes

Total number of bytes belonging to lost data units.

### Data Loss

The final data-loss percentage is calculated using bytes:

```text
Data Loss =
(Lost Bytes / Original Message Bytes) × 100
```

### Average Latency

Average latency is calculated using successfully received data units:

```text
Average Latency =
Total Measured Latency / Received Units
```

---

# 🧪 Example Test

Suppose the user configures:

```text
Packet Loss       : 10%
Simulated Latency : 100 ms
```

Then enters:

```text
hello akash this is my network testing project
```

The client divides the message into data units.

During the test, one data unit may be randomly selected for loss.

Example result:

```text
========== NETWORK TEST RESULT ==========
Original Data     : hello akash this is my network testing project
Data Units        : 10
Received Units    : 9
Lost Units        : 1
Received Bytes    : 41
Lost Bytes        : 5
Data Loss         : 10.8696%
Average Latency   : approximately 100+ ms
==========================================
```

The exact result can vary because the lost data unit is selected randomly.

The measured latency can also be slightly higher than the configured value due to actual processing and communication overhead.

---

# 🧪 Test Scenarios

## Test Case 1 — Normal Network

```text
Packet Loss       : 0%
Simulated Latency : 0 ms
```

Expected behavior:

- No intentional data-unit loss.
- All data units should normally reach the server.
- Average latency should remain low.

---

## Test Case 2 — Low Packet Loss

```text
Packet Loss       : 10%
Simulated Latency : 0 ms
```

Expected behavior:

- Approximately 10% of the data units are selected for loss.
- Server identifies the missing units.
- Lost and received units are displayed.

---

## Test Case 3 — Moderate Packet Loss

```text
Packet Loss       : 30%
Simulated Latency : 0 ms
```

Expected behavior:

- Approximately 30% of data units are selected for loss.
- The reconstructed message contains `[LOST]` markers where units are missing.

---

## Test Case 4 — High Latency

```text
Packet Loss       : 0%
Simulated Latency : 100 ms
```

Expected behavior:

- No intentional data-unit loss.
- Each transmitted unit experiences simulated delay.
- Average measured latency increases.

---

## Test Case 5 — Combined Conditions

```text
Packet Loss       : 30%
Simulated Latency : 100 ms
```

Expected behavior:

- Some data units are intentionally lost.
- Successfully transmitted units experience delay.
- Final results show both data loss and latency.

---

## Test Case 6 — Complete Loss

```text
Packet Loss       : 100%
Simulated Latency : 0 ms
```

Expected behavior:

- All data units are intentionally dropped by the client.
- No DATA units reach the server.
- Server identifies all expected units as lost.

---

# 📁 Project Structure

```text
network-chaos-emulator/
│
├── src/
│   ├── main.cpp
│   ├── client.cpp
│   └── server.cpp
│
├── results/
│   └── last_test.txt
│
├── chaos_emulator
├── client
├── server
│
└── .gitignore
```

---

## File Description

| File | Purpose |
|---|---|
| `src/main.cpp` | Main menu and network configuration |
| `src/client.cpp` | Message processing, UDP transmission, loss/latency simulation and measurement |
| `src/server.cpp` | UDP receiver, ACK handling, missing-unit detection and reconstruction |
| `results/last_test.txt` | Stores the latest network test result locally |
| `chaos_emulator` | Compiled main controller |
| `client` | Compiled client executable |
| `server` | Compiled server executable |
| `.gitignore` | Prevents selected build files from being tracked |

---

# 🛠️ Technologies Used

## Programming Language

- C++

## Networking

- UDP
- IPv4
- Linux Socket API
- Application-level ACK

## Operating Environment

- Ubuntu
- WSL2

## Development Tools

- GNU G++
- Git
- GitHub
- Linux Terminal

---

# 📚 C++ Concepts Used

The project uses several fundamental C++ concepts:

- Variables
- Data types
- Conditional statements
- Loops
- Functions
- Strings
- Arrays
- Vectors
- File handling
- Random number generation
- Time measurement
- Command-line arguments
- Type conversion
- Basic error handling
- Linux socket programming

---

# 💻 System Requirements

The project requires:

- Ubuntu or WSL2
- GNU G++ compiler
- Linux socket libraries
- Git

Check the compiler:

```bash
g++ --version
```

Check Git:

```bash
git --version
```

---

# 🚀 Installation

Clone the repository:

```bash
git clone https://github.com/Akashbit13/Network-Latency-Packet-Loss-Chaos-Emulator.git
```

Move into the project:

```bash
cd Network-Latency-Packet-Loss-Chaos-Emulator
```

---

# 🔨 Compilation

Compile the main controller:

```bash
g++ src/main.cpp -o chaos_emulator
```

Compile the client:

```bash
g++ src/client.cpp -o client
```

Compile the server:

```bash
g++ src/server.cpp -o server
```

After successful compilation, the project will have:

```text
chaos_emulator
client
server
```

---

# ▶️ Running the Project

The server should be started first.

## Terminal 1

Run:

```bash
./server
```

Expected output:

```text
UDP Server started on port 8080.
Waiting for network test...
```

Keep this terminal running.

---

## Terminal 2

Open another terminal in the same project directory and run:

```bash
./chaos_emulator
```

The main application menu will appear.

---

# 🔁 Example Execution Flow

```text
1. Start the server

        ↓

2. Start ./chaos_emulator

        ↓

3. Select:
   Configure Network Conditions

        ↓

4. Configure:
   Packet Loss = 10%
   Latency = 100 ms

        ↓

5. Select:
   Start New Network Test

        ↓

6. Enter:
   hello akash this is my network testing project

        ↓

7. Client divides the message into data units

        ↓

8. Loss and latency conditions are applied

        ↓

9. Data is transmitted using UDP

        ↓

10. Server receives available data units

        ↓

11. Server sends ACK

        ↓

12. Client calculates results

        ↓

13. Result is displayed

        ↓

14. Latest result is saved locally
```

---

# 💾 Result Storage

After a test is completed, the latest result is stored in:

```text
results/last_test.txt
```

Example:

```text
========== LAST NETWORK TEST ==========
Original Data     : hello akash this is my network testing project
Packet Loss       : 10%
Simulated Latency : 100 ms
Data Units        : 10
Received Units    : 9
Lost Units        : 1
Received Bytes    : 41
Lost Bytes        : 5
Data Loss         : 10.8696%
Average Latency   : 106.2 ms
=======================================
```

The application can display this result again through:

```text
4. View Last Test Results
```

---

# 🔄 Multiple Tests

The server is designed to remain active after completing a test.

After one test:

```text
Test 1
  ↓
Server displays result
  ↓
Server clears previous test data
  ↓
Server waits for next test
  ↓
Test 2
```

This allows multiple network tests to be performed without restarting the server.

---

# 🧠 Design Decisions

## 1. UDP Instead of TCP

UDP was selected because it provides a simple datagram-based communication model and does not automatically retransmit intentionally missing data.

This makes it suitable for demonstrating data loss.

---

## 2. Application-Level Loss

The project intentionally drops selected data units inside the client before they are transmitted.

This approach was selected because it is easier to understand and demonstrate than modifying the operating system's actual network traffic.

---

## 3. Fixed Data-Unit Size

A data-unit size of 5 bytes is used.

The small size makes it easier to see individual units being received or lost.

---

## 4. Simple Text Protocol

A simple protocol such as:

```text
DATA|2|10|Hello
```

was selected because it is easy to read and debug.

A binary protocol could be used in a larger system, but it would add unnecessary complexity for this project.

---

## 5. Application-Level ACK

The server sends an ACK after receiving a data unit.

This allows the client to determine whether the data unit reached the server.

---

## 6. Text File for Results

Only the latest result needs to be stored.

Therefore, a simple text file was selected instead of a database.

This keeps the project lightweight and easy to understand.

---

# ⚠️ Limitations

The current project intentionally has a limited scope.

### Current limitations include:

- Communication is tested on localhost.
- Client and server run on the same machine.
- Network conditions are simulated at the application level.
- The project does not modify actual system network traffic.
- The project does not implement kernel-level network emulation.
- Automatic retransmission is not implemented.
- Only the latest result is stored.
- Maximum supported data units are limited to 100.
- The project is command-line based.
- No database is used.
- No graphical user interface is included.

These limitations are intentional to keep the implementation focused on fundamental C++ programming, UDP communication and application resilience concepts.

---

# 🔮 Future Enhancements

The project can be extended in the future with features such as:

- Communication between different machines
- Configurable data-unit size
- Test history storage
- More network condition profiles
- Additional network performance metrics
- TCP vs UDP comparison
- Detailed logging
- Result visualization
- Graphical user interface
- More advanced network testing scenarios

---

# 🎓 Learning Outcomes

This project provided practical understanding of:

### C++

- Object-independent procedural program structure
- Variables and data types
- Loops
- Conditions
- Strings
- Arrays
- Vectors
- File handling
- Random number generation
- Time measurement

### Linux

- Linux terminal
- Compilation using G++
- Running executable files
- File system navigation
- Basic socket programming environment

### Networking

- UDP
- IPv4
- IP addresses
- Ports
- Client-server communication
- Socket creation
- `bind()`
- `sendto()`
- `recvfrom()`
- ACK-based communication
- Data loss
- Latency

### Development

- Git
- GitHub
- Source code organization
- Compilation and debugging
- Testing
- Documentation

---

# 🧪 Testing Approach

The project can be tested by changing packet-loss and latency conditions and observing the resulting behavior.

Example test matrix:

| Test | Packet Loss | Latency | Main Observation |
|---|---:|---:|---|
| Normal | 0% | 0 ms | Normal transmission |
| Low Loss | 10% | 0 ms | Small amount of data loss |
| Moderate Loss | 30% | 0 ms | More missing data units |
| High Loss | 70% | 0 ms | Majority of data units lost |
| Complete Loss | 100% | 0 ms | All data units lost |
| Low Delay | 0% | 20 ms | Small transmission delay |
| Moderate Delay | 0% | 50 ms | Increased latency |
| High Delay | 0% | 100 ms | Noticeable latency |
| Combined | 30% | 100 ms | Loss and latency together |

---

# 📌 Project Scope

The main focus of this project is:

> **Demonstrating how controlled application-level network loss and latency can affect data transmission using C++ and UDP.**

The project is intended for:

- Academic learning
- Training
- Network programming practice
- Application resilience demonstrations
- Basic network testing concepts

It is not intended to replace professional network testing or traffic-emulation tools.

---

# 🔐 Safety and Simplicity

The project is designed as a local educational tool.

It operates using localhost UDP communication and does not modify the operating system's actual network configuration.

No external network traffic is intentionally affected by the application.

---

# 📈 Expected Outcome

After running the project, the user should be able to observe:

```text
Configured Conditions
        |
        +---- Packet/Data Loss
        |
        +---- Simulated Latency
        |
        v
UDP Data Transmission
        |
        v
Server Reception
        |
        +---- Received Units
        |
        +---- Lost Units
        |
        +---- Received Bytes
        |
        v
Performance Measurement
        |
        +---- Data Loss %
        |
        +---- Average Latency
```

This demonstrates the basic relationship between network conditions and application-level communication.

---

# 👨‍💻 Author

## Akash Patra

**B.Tech – Computer Science and Engineering**

### Project

**Network Latency & Packet-Loss Chaos Emulator**

### Project Focus

**Application Resilience Testing**

---

# 🔗 Repository

GitHub Repository:

https://github.com/Akashbit13/Network-Latency-Packet-Loss-Chaos-Emulator

---

# ⭐ Summary

The **Network Latency & Packet-Loss Chaos Emulator** is a lightweight C++ project that demonstrates application resilience testing through controlled UDP communication.

The application allows users to configure data loss and latency, transmit a complete message as smaller data units, observe successful and lost units, measure latency, and analyze the final transmission result.

The project combines fundamental concepts of:

**C++ + Linux + UDP Socket Programming + Client-Server Architecture + Network Resilience Testing**

while keeping the implementation simple, transparent and suitable for academic demonstration.
