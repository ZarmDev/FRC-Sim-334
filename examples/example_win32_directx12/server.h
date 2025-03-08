#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;

class Server {
public:
    void server_init();
    void serverCleanup();
};

#endif // SERVER_H
