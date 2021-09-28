#pragma once

#include <string>
#include <array>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Vector2.hpp>

#include "snake_settings.h"
#include "system.h"

namespace snake
{

    class SnakeClient : public System
    {
    public:
        sf::Socket::Status Connect(sf::IpAddress address, unsigned short portNumber);
        SnakePhase GetPhase() const;
        bool IsConnected() const;
        void Init() override;
        void ReceivePacket(sf::Packet& packet);
        void Update() override;
        void Destroy() override;
        int GetPlayerNumber() const;
        void SendNewMove(int position);

        void SendNewRoll();
        const std::array<Move, 50>& GetMoves() const;
        unsigned char GetMoveIndex() const;
        std::string_view GetEndMessage() const;
    std::array<Move, 50> moves_{};
    	private:
        sf::TcpSocket socket_;
        SnakePhase phase_ = SnakePhase::CONNECTION;
        
        unsigned char currentMoveIndex_ = 0;
        std::string endMessage_;
        PlayerNumber playerNumber_ = 255u;
    	
 
    };

   

    class SnakeView : public DrawImGuiInterface
    {
    public:
        SnakeView(SnakeClient& client);
        void DrawImGui() override;
    private:
        SnakeClient& client_;
        std::string ipAddressBuffer_ = "localhost";
        int portNumber_ = serverPortNumber;
        std::array<int, 50> currentPosition_{};
    };
}