#include "client.h"


int main()
{
  EmotionClient client;
  client.SetInfo();
  client.Connect();
  while (true)
  {
    client.loop();
  }

  return 0;
}