// WINRT (socket programming)
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>

using namespace winrt;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::Foundation::Collections;

//namespace beast = boost::beast;     // from <boost/beast.hpp>
//namespace http = beast::http;       // from <boost/beast/http.hpp>
//namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
//namespace net = boost::asio;        // from <boost/asio.hpp>
//using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

#pragma comment(lib, "Ws2_32.lib")


        //WSADATA wsaData;
        //SOCKET ConnectSocket = INVALID_SOCKET;
        //struct addrinfo* result = NULL, * ptr = NULL, hints;
        //const char* sendbuf = "Hello from client";
        //char recvbuf[512];
        //int iResult;
        //int recvbuflen = 512;

        //// Initialize Winsock
        //iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        //if (iResult != 0) {
        //    std::cerr << "WSAStartup failed: " << iResult << std::endl;
        //    return 1;
        //}

        //ZeroMemory(&hints, sizeof(hints));
        //hints.ai_family = AF_UNSPEC;
        //hints.ai_socktype = SOCK_STREAM;
        //hints.ai_protocol = IPPROTO_TCP;

        //// Resolve the server address and port
        //iResult = getaddrinfo(SIM_ADDRESS, PORT, &hints, &result);
        //if (iResult != 0) {
        //    std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        //    WSACleanup();
        //    return 1;
        //}

        //// Attempt to connect to an address until one succeeds
        //for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        //    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        //    if (ConnectSocket == INVALID_SOCKET) {
        //        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        //        WSACleanup();
        //        return 1;
        //    }

        //    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        //    if (iResult == SOCKET_ERROR) {
        //        closesocket(ConnectSocket);
        //        ConnectSocket = INVALID_SOCKET;
        //        continue;
        //    }
        //    break;
        //}

        //freeaddrinfo(result);

        //if (ConnectSocket == INVALID_SOCKET) {
        //    std::cerr << "Unable to connect to server!" << std::endl;
        //    WSACleanup();
        //    return 1;
        //}

        //// Send an initial buffer
        //iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        //if (iResult == SOCKET_ERROR) {
        //    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        //    closesocket(ConnectSocket);
        //    WSACleanup();
        //    return 1;
        //}

        //std::cout << "Bytes Sent: " << iResult << std::endl;

        //try {
        //    // The io_context is required for all I/O
        //    net::io_context ioc;

        //    // These objects perform our I/O
        //    tcp::resolver resolver(ioc);
        //    websocket::stream<tcp::socket> ws(ioc);

        //    // Look up the domain name
        //    auto const results = resolver.resolve("echo.websocket.org", "80");

        //    // Make the connection on the IP address we get from a lookup
        //    net::connect(ws.next_layer(), results.begin(), results.end());

        //    // Perform the WebSocket handshake
        //    ws.handshake("echo.websocket.org", "/");

        //    // Send a message
        //    ws.write(net::buffer(std::string("Hello, WebSocket!")));

        //    // This buffer will hold the incoming message
        //    beast::flat_buffer buffer;

        //    // Read a message into our buffer
        //    ws.read(buffer);

        //    // Print the message
        //    std::cout << beast::make_printable(buffer.data()) << std::endl;

        //    // Close the WebSocket connection
        //    ws.close(websocket::close_code::normal);
        //}
        //catch (std::exception const& e) {
        //    std::cerr << "Error: " << e.what() << std::endl;
        //    return EXIT_FAILURE;
        //}
#include "server.h"

void Server::server_init() {
    init_apartment();

    // Generate UUID
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1, 9999999999);
    int UUID = distribution(generator);

    // Important variables
    std::string serverBaseAddr = "127.0.0.1";
    int port = 5810;
    std::string prefix = "ws://";
    std::ostringstream oss;
    oss << prefix << serverBaseAddr << ":" << port << "/nt/FRC-sim_" << UUID;
    std::string serverAddr = oss.str();

    // Create a MessageWebSocket object
    MessageWebSocket webSocket;

    // Set the protocol for the WebSocket connection
    webSocket.Control().SupportedProtocols().Append(L"v4.1.networktables.first.wpi.edu");

    // Attach the MessageReceived event handler
    webSocket.MessageReceived([](MessageWebSocket const&, MessageWebSocketMessageReceivedEventArgs const& args) {
        DataReader reader = args.GetDataReader();
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
        auto message = reader.ReadString(reader.UnconsumedBufferLength());
        std::string messageStr = winrt::to_string(message);
        std::cout << messageStr;

        if (messageStr[0] == '{') {
            // Handle JSON message
            //handleJsonMessage(messageStr);
        }
        else {
            // Handle binary message
            std::vector<uint8_t> data(messageStr.begin(), messageStr.end());
            std::cout << data[0];
            //handleBinaryMessage(data);
        }
        });

    // Connect to the WebSocket server
    try {
        // Connect to the WebSocket server
        auto connectOperation = webSocket.ConnectAsync(winrt::Windows::Foundation::Uri(winrt::to_hstring(serverAddr)));
        connectOperation.Completed([webSocket](auto const& asyncInfo, auto const asyncStatus) {
            try {
                if (asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed) {
                    asyncInfo.GetResults();
                    std::wcout << L"Connected to WebSocket server!" << std::endl;

                    // Variables
                    int topicId;
                    int serverTime;
                    //typeInfo : List[int, str]
                    //value : Union[bool, int, float, str, bytes, bytearray, list]

                    // Dump as JSON.string
                    /*binary_data = BinaryMessageData(
                        topicId = topic_id,
                        serverTime = int(time.time() * 1000),
                        typeInfo = type_info,
                        value = value
                    )*/

                    // Send a test message to the server
                    DataWriter writer(webSocket.OutputStream());
                    writer.WriteString(L"Hello, server!");
                    writer.StoreAsync().get();
                    std::wcout << L"Test message sent to server." << std::endl;

                    // Keep the application running to receive messages
                    std::cin.get();
                }
                else {
                    throw winrt::hresult_error(asyncInfo.ErrorCode());
                }
            }
            catch (const winrt::hresult_error& ex) {
                std::wcerr << L"WebSocket connection failed: " << ex.message().c_str() << std::endl;
                std::wcerr << L"HRESULT: " << std::hex << ex.code() << std::endl;
            }
            catch (const std::exception& ex) {
                std::wcerr << L"WebSocket connection failed: " << ex.what() << std::endl;
            }
            });
    }
    catch (const winrt::hresult_error& ex) {
        std::wcerr << L"WebSocket connection failed: " << ex.message().c_str() << std::endl;
        std::wcerr << L"HRESULT: " << std::hex << ex.code() << std::endl;
    }
    catch (const std::exception& ex) {
        std::wcerr << L"WebSocket connection failed: " << ex.what() << std::endl;
    }
}

void Server::serverCleanup() {
    //closesocket(ConnectSocket);
    //WSACleanup();
}
