// WINRT (socket programming)
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>

// Standard library
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <variant>
#include <stdexcept>

// Cereal
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>

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

#pragma comment(lib, "Ws2_32.lib")

#include "server.h"
#include <chrono>

class NetworkTablesTypeInfo {
public:
    static inline const std::vector<int> kBoolean = { 0, 'b' };
    static inline const std::vector<int> kDouble = { 1, 'd' };
    static inline const std::vector<int> kInteger = { 2, 'i' };
    static inline const std::vector<int> kString = { 4, 's' };
    static inline const std::vector<int> kArrayBuffer = { 3, 'r' };
    static inline const std::vector<int> kBooleanArray = { 16, 'B' };
    static inline const std::vector<int> kDoubleArray = { 17, 'D' };
    static inline const std::vector<int> kIntegerArray = { 18, 'I' };
    static inline const std::vector<int> kStringArray = { 20, 'S' };

    static std::vector<int> get_network_tables_type_from_object(const NTVariant& data) {
        if (std::holds_alternative<bool>(data)) return kBoolean;
        if (std::holds_alternative<int>(data)) return kInteger;
        if (std::holds_alternative<double>(data)) return kDouble;
        if (std::holds_alternative<std::string>(data)) return kString;
        if (std::holds_alternative<std::vector<bool>>(data)) return kBooleanArray;
        if (std::holds_alternative<std::vector<int>>(data)) return kIntegerArray;
        if (std::holds_alternative<std::vector<double>>(data)) return kDoubleArray;
        if (std::holds_alternative<std::vector<std::string>>(data)) return kStringArray;
        throw std::invalid_argument("Invalid data for NT");
    }

    static std::vector<int> get_network_tables_type_from_type_num(int type_num) {
        switch (type_num) {
        case 0: return kBoolean;
        case 1: return kDouble;
        case 2: return kInteger;
        case 4: return kString;
        case 3: return kArrayBuffer;
        case 16: return kBooleanArray;
        case 17: return kDoubleArray;
        case 18: return kIntegerArray;
        case 20: return kStringArray;
        default: throw std::invalid_argument("Invalid type number");
        }
    }
};

struct BinaryMessageData {
private:
    int topicId;
    int serverTime;
    std::vector<int> typeInfo;
    NTVariant value;

    // To allow the class to be serialized by cereal
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(topicId, serverTime, typeInfo, value);
    }
public:
    BinaryMessageData(int topicId, int serverTime, std::vector<int> typeInfo,
        NTVariant value)
        : topicId(topicId), serverTime(serverTime), typeInfo(typeInfo), value(value) {
        if (typeInfo.size() != 2) {
            throw std::invalid_argument("Invalid typeInfo format");
        }

        int type_num = typeInfo[0];
        auto type_info = NetworkTablesTypeInfo::get_network_tables_type_from_type_num(type_num);

        if (type_info == NetworkTablesTypeInfo::kBoolean && !std::holds_alternative<bool>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected bool)");
        }
        else if (type_info == NetworkTablesTypeInfo::kDouble && !std::holds_alternative<double>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected double)");
        }
        else if (type_info == NetworkTablesTypeInfo::kInteger && !std::holds_alternative<int>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected int)");
        }
        else if (type_info == NetworkTablesTypeInfo::kString && !std::holds_alternative<std::string>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected string)");
        }
        else if (type_info == NetworkTablesTypeInfo::kBooleanArray && !std::holds_alternative<std::vector<bool>>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected list of bools)");
        }
        else if (type_info == NetworkTablesTypeInfo::kDoubleArray && !std::holds_alternative<std::vector<double>>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected list of doubles)");
        }
        else if (type_info == NetworkTablesTypeInfo::kIntegerArray && !std::holds_alternative<std::vector<int>>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected list of ints)");
        }
        else if (type_info == NetworkTablesTypeInfo::kStringArray && !std::holds_alternative<std::vector<std::string>>(value)) {
            throw std::invalid_argument("Value type mismatch with typeInfo (expected list of strings)");
        }
    }
};

int64_t getCurrentTimeMillis() {
    auto now = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    return milliseconds; // No need to cast to int
}

int getTopicID(string key) {
    return 0;
}

void Server::updateValue(string key, NTVariant value, MessageWebSocket& webSocket) {
    vector<int> type_info = NetworkTablesTypeInfo::get_network_tables_type_from_object(value);
    int topic_id = getTopicID(key);
    BinaryMessageData binary_data = BinaryMessageData(topic_id, getCurrentTimeMillis(), type_info, value);

    // Serialize the object to a string
    std::ostringstream oss;
    cereal::BinaryOutputArchive oa(oss);
    oa(binary_data);
    std::string serialized_message = oss.str();

    // Convert the serialized string to a byte array
    std::vector<uint8_t> byte_array(serialized_message.begin(), serialized_message.end());

    // Create a DataWriter to write to the OutputStream
    DataWriter writer(webSocket.OutputStream());

    // Write the byte array to the DataWriter
    writer.WriteBytes(byte_array);

    writer.StoreAsync().get();
    std::wcout << L"Test message sent to server." << std::endl;
}

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

    try {
        std::vector<int> typeInfo = NetworkTablesTypeInfo::kBoolean;
        BinaryMessageData data(1, getCurrentTimeMillis(), typeInfo, true);
        std::cout << "BinaryMessageData created successfully" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    // Connect to the WebSocket server
    try {
        // Connect to the WebSocket server
        auto connectOperation = webSocket.ConnectAsync(winrt::Windows::Foundation::Uri(winrt::to_hstring(serverAddr)));
        connectOperation.Completed([this, &webSocket](auto const& asyncInfo, auto const asyncStatus) {
            try {
                if (asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed) {
                    asyncInfo.GetResults();
                    std::wcout << L"Connected to WebSocket server!" << std::endl;

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
    //webSocket.Close();
}
