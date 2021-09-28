#pragma once
#include <SFML/System/Vector2.hpp>

namespace snake
{
constexpr int maxClientNmb = 2;
constexpr unsigned short serverPortNumber = 33333;
enum class SnakePhase
{
    CONNECTION,
    GAME,
    END
};

using PlayerNumber = unsigned char;
using PlayerMove = unsigned int;
struct Move
{
    int position;
    PlayerNumber playerNumber;
};
}
