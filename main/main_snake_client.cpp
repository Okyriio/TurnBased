

#include "engine.h"
#include "snake_client.h"

int main()
{
    morpion::MorpionClient client;
    morpion::MorpionView view(client);
    Engine engine;
    engine.AddDrawImGuiSystem(&view);
    engine.AddSystem(&client);
    engine.Run();
    return 0;
}
