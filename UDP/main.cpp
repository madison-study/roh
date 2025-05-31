#include <iostream>
#include <string>
#include "client.h"
#include "server.h"

int main()
{
    int choice;
    std::cout << "Enter 1 to run as client, 2 to run as server: ";
    std::cin >> choice;

    if (choice == 1)
    {
        std::string ip;
        uint16_t port;
        std::string message;

        std::cout << "Enter server IP: ";
        std::cin >> ip;
        std::cout << "Enter server port: ";
        std::cin >> port;
        std::cin.ignore(); // flush newline left in buffer

        UdpClient client(ip, port);
        uint32_t seq = 0;
        while (true)
        {
            std::cout << "Enter message(1: normal 2: exception) to send (or 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit")
                break;

            if (message == "1")
            {
                for (int i = 0; i < 15; i++)
                {   
                    client.sendMessage(std::to_string(seq++) + ":" + std::to_string(i));
                }
            }

            if (message == "2")
            {
                for (int i = 0; i < 15; i++)
                {   
                    if(i % 3 == 0) {seq++; continue;}
                    client.sendMessage(std::to_string(seq++) + ":" + std::to_string(i));
                }
            }
        }
    }
    else if (choice == 2)
    {
        uint16_t port;
        std::cout << "Enter port to listen on: ";
        std::cin >> port;

        UdpServer server(port);

        while (true)
        {
            server.receiveMessage();
        }
    }
    else
    {
        std::cerr << "Invalid choice." << std::endl;
    }

    return 0;
}