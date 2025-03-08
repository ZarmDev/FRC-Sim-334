#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <random>
#include <sstream>
#include <variant>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using NTVariant = std::variant<
    bool,
    int,
    double,
    std::string,
    std::vector<bool>,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;
using namespace std;

class Server {
public:
    void server_init();
    void serverCleanup();
    void updateValue(std::string key, NTVariant value);
private:
    vector<NTVariant> entries;
    // Create a MessageWebSocket object
    MessageWebSocket webSocket;
};

#endif // SERVER_H
