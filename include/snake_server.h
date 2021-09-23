#pragma once

#include <array>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <snake_settings.h>

#include "snake_packet.h"

namespace snake
{

class SnakeServer
{
public:
    int Run();
private:
    std::array<sf::TcpSocket, maxClientNmb> sockets_;
    std::array<Move, 9> moves_{};
    sf::SocketSelector selector_;
    sf::TcpListener listener_;
    SnakePhase phase_ = SnakePhase::CONNECTION;
    unsigned char currentMoveIndex_ = 0;

    void StartNewGame();
    void UpdateConnectionPhase();
    void UpdateGamePhase();
    void UpdateEndPhase();
    void ReceivePacket();
    PlayerNumber CheckWinner() const;
    void ManageMovePacket(const MovePacket& movePacket);

    int GetNextSocket();
};
}
