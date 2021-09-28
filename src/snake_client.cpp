#include "snake_client.h"

#include <array>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <iostream>
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/Socket.hpp>

#include "snake_packet.h"
#include "snake_server.h"
#include "snake_settings.h"

namespace snake
{
    sf::Socket::Status SnakeClient::Connect(sf::IpAddress address, unsigned short portNumber)
    {
        const auto status = socket_.connect(address, portNumber);
        socket_.setBlocking(false);
        return status;
    }

    SnakePhase SnakeClient::GetPhase() const
    {
        return phase_;
    }

    bool SnakeClient::IsConnected() const
    {
        return socket_.getLocalPort() != 0;
    }

    void SnakeClient::Init()
    {
    }

    void SnakeClient::ReceivePacket(sf::Packet& packet)
    {
        Packet snakePacket{};
        packet >> snakePacket;

        switch (static_cast<PacketType>(snakePacket.packetType))
        {
        case PacketType::GAME_INIT:
        {
            GameInitPacket gameInitPacket{};
            packet >> gameInitPacket;
            playerNumber_ = gameInitPacket.playerNumber;
            phase_ = SnakePhase::GAME;
            std::cout << "You are player " << gameInitPacket.playerNumber + 1 << '\n';
            break;
        }
        case PacketType::MOVE:
        {
            if (phase_ != SnakePhase::GAME)
                break;
            RollPacket movePacket;
            packet >> movePacket;
            std::cout << "Receive move packet from player ";
            auto& currentMove = moves_[currentMoveIndex_];
           
            currentMove.playerNumber = movePacket.playerNumber;
            currentMoveIndex_++;
            break;
        }
        case PacketType::END:
        {
            if (phase_ != SnakePhase::GAME)
            {
                break;
            }
            EndPacket endPacket;
            packet >> endPacket;
            switch (endPacket.endType)
            {
            case EndType::STALEMATE:
                endMessage_ = "Stalemate";
                break;
            case EndType::WIN_P1:
                endMessage_ = playerNumber_ == 0 ? "You won" : "You lost";
                break;
            case EndType::WIN_P2:
                endMessage_ = playerNumber_ == 1 ? "You won" : "You lost";
                break;
            case EndType::ERROR:
                endMessage_ = "Error";
                break;
            default:;
            }
            phase_ = SnakePhase::END;
            break;
        }
        default:
            break;
        }
    }

    void SnakeClient::Update()
    {
        //Receive packetS
        if (socket_.getLocalPort() != 0)
        {
            sf::Packet receivedPacket;
            sf::Socket::Status status;
            do
            {
                status = socket_.receive(receivedPacket);
            } while (status == sf::Socket::Partial);

            if (status == sf::Socket::Done)
            {
                ReceivePacket(receivedPacket);
            }

            if (status == sf::Socket::Disconnected)
            {
                socket_.disconnect();
                std::cerr << "Server disconnected\n";
            }
        }
    }

    void SnakeClient::Destroy()
    {
    }

    int SnakeClient::GetPlayerNumber() const
    {
        return playerNumber_;
    }

    void SnakeClient::SendNewMove(int position)
    {
        RollPacket movePacket;
        movePacket.playerNumber = playerNumber_;
        sf::Packet packet;
        packet << movePacket;
        sf::Socket::Status sentStatus;
        do
        {
            sentStatus = socket_.send(packet);
        } while (sentStatus == sf::Socket::Partial);
    }

    const std::array<Move, 50>& SnakeClient::GetMoves() const
    {
        return moves_;
    }

    unsigned char SnakeClient::GetMoveIndex() const
    {
        return currentMoveIndex_;
    }

    std::string_view SnakeClient::GetEndMessage() const
    {
        return endMessage_;
    }

    SnakeView::SnakeView(SnakeClient& client) : client_(client)
    {
    }

    void SnakeClient::SendNewRoll()
    {
        RollPacket rollPacket;
        rollPacket.packetType = PacketType::DRAW;
        sf::Packet packet;
        packet << rollPacket;
        sf::Socket::Status sentStatus;
        do
        {
            sentStatus = socket_.send(packet);
        } while (sentStatus == sf::Socket::Partial);
    }
  
    void SnakeView::DrawImGui()
    {
        switch (client_.GetPhase())
        {
        case SnakePhase::CONNECTION:
        {
            if (client_.IsConnected())
                return;
            ImGui::Begin("Client");

            ImGui::InputText("Ip Address", &ipAddressBuffer_);
            ImGui::InputInt("Port Number", &portNumber_);
            if (ImGui::Button("Connect"))
            {
                const auto status = client_.Connect(sf::IpAddress(ipAddressBuffer_), portNumber_);
                if (status != sf::Socket::Done)
                {
                    switch (status)
                    {
                    case sf::Socket::NotReady:
                        std::cerr << "Not ready to connect to " << ipAddressBuffer_ << ':' << portNumber_ << '\n';
                        break;
                    case sf::Socket::Partial:
                        std::cerr << "Connecting to " << ipAddressBuffer_ << ':' << portNumber_ << '\n';
                        break;
                    case sf::Socket::Disconnected:
                        std::cerr << "Disconnecting to " << ipAddressBuffer_ << ':' << portNumber_ << '\n';
                        break;
                    case sf::Socket::Error:
                        std::cerr << "Error connecting to " << ipAddressBuffer_ << ':' << portNumber_ << '\n';
                        break;
                    default:;
                    }
                }
                else
                {
                    std::cout << "Successfully connected to server\n";
                }

            }
            ImGui::End();
            break;




        case SnakePhase::GAME:
        {
            const auto playerNumber = client_.GetPlayerNumber();
            ImGui::Begin("Client");
            ImGui::Text("You are player %d", playerNumber + 1);

            std::array<char, 25> board;
            board.fill(' ');
            board[24] = 0;
            const auto& moves = client_.GetMoves();

            ImGui::Text("%s", board.data());


            
        }
        case SnakePhase::END:
        {
            ImGui::Begin("Client");
            ImGui::Text("%s", client_.GetEndMessage().data());
            ImGui::End();
            break;
        }
        default:;
        }



        }
    }
}