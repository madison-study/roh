#include <iostream>
#include <string>
#include "client.h"
#include "server.h"

int main()
{
    int choice;
    std::cout << "Enter 1 to run as client, 2 to run as server(no prefix), 3 to run as server(prefix): ";
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

        TCPClient client(ip, port);
        uint32_t seq = 0;
        while (true)
        {
            std::cout << "Enter message(1: prefix 2: no prefix) to send (or 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit")
                break;

            if (message == "kill")
                exit(0);

            if (message == "1")
            {
                for (int i = 0; i < 15; i++)
                {   
                    client.sendMessage(std::to_string(i),1);
                }
            }

            if (message == "2")
            {
                for (int i = 0; i < 15; i++)
                {   
                    client.sendMessage(std::to_string(i),0);
                }
            }
        }
    }
    else if (choice == 2 || choice == 3)
    {
        uint16_t port;
        std::cout << "Enter port to listen on: ";
        std::cin >> port;

        TCPServer server(port);

        while (true)
        {
            server.receiveMessage(choice % 2);
        }
    }
    else
    {
        std::cerr << "Invalid choice." << std::endl;
    }

    return 0;
}