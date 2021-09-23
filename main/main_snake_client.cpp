

#include "engine.h"
#include "snake_client.h"

int main()
{
    snake::SnakeClient client;
    snake::SnakeView view(client);
    Engine engine;
    engine.AddDrawImGuiSystem(&view);
    engine.AddSystem(&client);
    engine.Run();
    return 0;
}
