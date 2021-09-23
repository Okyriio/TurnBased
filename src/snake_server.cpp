#include "snake_server.h"
#include <SFML/Network/TcpSocket.hpp>
#include <iostream>
#include <random>
#include "snake_packet.h"


namespace snake
{
    void SnakeServer::ReceivePacket()
    {
        if (selector_.wait(sf::milliseconds(20)))
        {
            for (auto& socket : sockets_)
            {
                if (selector_.isReady(socket))
                {
                    sf::Packet receivedPacket;
                    sf::Socket::Status receivingStatus;
                    do
                    {
                        receivingStatus = socket.receive(receivedPacket);
                    } while (receivingStatus == sf::Socket::Partial);

                    Packet statusPacket;
                    receivedPacket >> statusPacket;
                    switch (static_cast<PacketType>(statusPacket.packetType))
                    {
                    case PacketType::MOVE:
                    {
                        MovePacket movePacket;
                        receivedPacket >> movePacket;
                        ManageMovePacket(movePacket);
                        break;
                    }
                    }

                }
            }
        }
    }




    void SnakeServer::ManageMovePacket(const MovePacket& movePacket)
    {
        std::cout << "Player " << movePacket.playerNumber + 1;

        if (phase_ != SnakePhase::GAME)
            return;

        if(currentMoveIndex_ % 2 != movePacket.playerNumber)
        {
            //TODO return to player an error msg
            return;
        }



        EndType endType = EndType::NONE;
       
        //TODO check victory condition
        PlayerNumber winningPlayer = 255u;
        if(winningPlayer != 255u)
        {
            endType = winningPlayer ? EndType::WIN_P2 : EndType::WIN_P1;
        }
        //TODO send end of game packet
        if(endType != EndType::NONE)
        {
            EndPacket endPacket{};
            endPacket.packetType = static_cast<unsigned char>(PacketType::END);
            endPacket.endType = endType;

            //sent new move to all players
            for (auto& socket : sockets_)
            {
                sf::Packet sentPacket;
                sentPacket << endPacket;
                sf::Socket::Status sentStatus;
                do
                {
                    sentStatus = socket.send(sentPacket);
                } while (sentStatus == sf::Socket::Partial);

            }

            phase_ = SnakePhase::END;
        }
        MovePacket newMovePacket = movePacket;
        newMovePacket.packetType = static_cast<unsigned char>(PacketType::MOVE);

        //sent new move to all players
        for(auto& socket: sockets_)
        {
            sf::Packet sentPacket;
            sentPacket << newMovePacket;
            sf::Socket::Status sentStatus;
            do
            {
                sentStatus = socket.send(sentPacket);
            } while (sentStatus == sf::Socket::Partial);
            
        }
    }

    int SnakeServer::Run()
    {
        if (listener_.listen(serverPortNumber) != sf::Socket::Done)
        {
            std::cerr << "[Error] Server cannot bind port: " << serverPortNumber << '\n';
            return EXIT_FAILURE;
        }
        std::cout << "Server bound to port " << serverPortNumber << '\n';

        while (true)
        {
            switch (phase_)
            {
            case SnakePhase::CONNECTION:
                ReceivePacket();
                UpdateConnectionPhase();
                break;
            case SnakePhase::GAME:
                ReceivePacket();
                UpdateGamePhase();
                break;
            case SnakePhase::END:
                return EXIT_SUCCESS;
            default:;
            }
        }
    }

    void SnakeServer::StartNewGame()
    {
        //Switch to Game state
        phase_ = SnakePhase::GAME;
        //Send game init packet
        std::cout << "Two players connected!\n";

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, 1);
        int dice_roll = distribution(generator);

        for (unsigned char i = 0; i < sockets_.size(); i++)
        {
            GameInitPacket gameInitPacket;
            gameInitPacket.packetType = static_cast<unsigned char>(PacketType::GAME_INIT);
            gameInitPacket.playerNumber = i != dice_roll;
            sf::Packet sentPacket;
            sentPacket << gameInitPacket;
            sf::Socket::Status sentStatus;
            do
            {
                sentStatus = sockets_[i].send(sentPacket);
            } while (sentStatus == sf::Socket::Partial);
        }
    }

    void SnakeServer::UpdateConnectionPhase()
    {
        // accept a new connection
        const auto nextIndex = GetNextSocket();

        if (nextIndex != -1)
        {
            auto& newSocket = sockets_[nextIndex];
            if (listener_.accept(newSocket) == sf::Socket::Done)
            {
                std::cout << "New connection from " <<
                    newSocket.getRemoteAddress().toString() << ':' << newSocket.
                    getRemotePort() << '\n';
                newSocket.setBlocking(false);
                selector_.add(newSocket);
                if (nextIndex == 1)
                {
                    StartNewGame();

                }
            }
        }
    }

    void SnakeServer::UpdateGamePhase()
    {
      
    }

    void SnakeServer::UpdateEndPhase()
    {
    }

    int SnakeServer::GetNextSocket()
    {
        for (int i = 0; i < maxClientNmb; i++)
        {
            if (sockets_[i].getLocalPort() == 0)
            {
                return i;
            }
        }
        return -1;
    }

}