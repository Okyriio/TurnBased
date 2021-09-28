#pragma once

#include <array>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <snake_settings.h>


#include "snake_client.h"
#include "snake_packet.h"

namespace snake
{

class SnakeServer
{
public:
    int Run();
    void UpdateGamePhase();
    static int Draw();
private:
    std::array<sf::TcpSocket, maxClientNmb> sockets_;
    std::array<Move, 25> moves_{};
    sf::SocketSelector selector_;
    sf::TcpListener listener_;
    SnakePhase phase_ = SnakePhase::CONNECTION;
    unsigned char currentMoveIndex_ = 0;
    
	
    void StartNewGame();
    void UpdateConnectionPhase();
  
    void UpdateEndPhase();
    void ReceivePacket();
    void ManageMovePacket(const RollPacket& rollPacket);

    int GetNextSocket();
    
};
}
