#include "client.h"
int main(int argc,char** argv)
{
    MY_CLIENT::My_client* client=MY_CLIENT::My_client::getInstance(IP,argv[1]);
    client->handle_page();
    return 0;
}
