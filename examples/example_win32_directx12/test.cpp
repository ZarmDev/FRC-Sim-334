#include <iostream>
#include <string>
#include <memory>
#include <functional>

// Using the websocketpp library for WebSocket functionality
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

// Type definitions for convenience
typedef websocketpp::client<websocketpp::config::asio_client> WebSocketClient;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::io_service> io_service_ptr;

class NetworkTablesClient {
private:
    WebSocketClient client;
    websocketpp::connection_hdl connection;
    io_service_ptr io_service;
    bool connected;

public:
    NetworkTablesClient() : connected(false) {
        // Set up logging
        client.set_access_channels(websocketpp::log::alevel::all);
        client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        client.init_asio();

        // Register event handlers
        client.set_open_handler(std::bind(&NetworkTablesClient::on_open, this, std::placeholders::_1));
        client.set_close_handler(std::bind(&NetworkTablesClient::on_close, this, std::placeholders::_1));
        client.set_fail_handler(std::bind(&NetworkTablesClient::on_fail, this, std::placeholders::_1));
        client.set_message_handler(std::bind(&NetworkTablesClient::on_message, this,
            std::placeholders::_1, std::placeholders::_2));
    }

    ~NetworkTablesClient() {
        if (connected) {
            disconnect();
        }
    }

    bool connect(const std::string& uri) {
        try {
            // Create a connection to the given URI
            websocketpp::lib::error_code ec;
            WebSocketClient::connection_ptr con = client.get_connection(uri, ec);

            if (ec) {
                std::cerr << "Could not create connection: " << ec.message() << std::endl;
                return false;
            }

            // Connect to the server
            client.connect(con);

            // Start the ASIO io_service run loop
            client.run();

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return false;
        }
        catch (...) {
            std::cerr << "Unknown exception" << std::endl;
            return false;
        }
    }

    void disconnect() {
        if (!connected) return;

        try {
            client.close(connection, websocketpp::close::status::normal, "");
            connected = false;
        }
        catch (const std::exception& e) {
            std::cerr << "Error on disconnect: " << e.what() << std::endl;
        }
    }

    bool send_message(const std::string& message) {
        if (!connected) return false;

        try {
            client.send(connection, message, websocketpp::frame::opcode::text);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error sending message: " << e.what() << std::endl;
            return false;
        }
    }

    // Subscribe to a specific Network Tables key
    bool subscribe(const std::string& table, const std::string& key) {
        if (!connected) return false;

        try {
            // Format subscription message according to NT4 protocol
            std::string subscription = "{\"operation\":\"subscribe\",\"topics\":[\"/" + table + "/" + key + "\"]}";
            return send_message(subscription);
        }
        catch (const std::exception& e) {
            std::cerr << "Error subscribing: " << e.what() << std::endl;
            return false;
        }
    }

    // Publish a value to Network Tables
    bool publish(const std::string& table, const std::string& key, const std::string& value) {
        if (!connected) return false;

        try {
            // Format publish message according to NT4 protocol
            std::string publish_msg = "{\"operation\":\"publish\",\"topic\":\"/" + table + "/" + key + "\",\"value\":" + value + "}";
            return send_message(publish_msg);
        }
        catch (const std::exception& e) {
            std::cerr << "Error publishing: " << e.what() << std::endl;
            return false;
        }
    }

private:
    void on_open(websocketpp::connection_hdl hdl) {
        std::cout << "Connection established!" << std::endl;
        connection = hdl;
        connected = true;
    }

    void on_close(websocketpp::connection_hdl hdl) {
        std::cout << "Connection closed!" << std::endl;
        connected = false;
    }

    void on_fail(websocketpp::connection_hdl hdl) {
        std::cout << "Connection failed!" << std::endl;
        connected = false;
    }

    void on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
        std::cout << "Received message: " << msg->get_payload() << std::endl;
        // Here you would typically parse the JSON message and handle the Network Tables data
    }
};

// Example usage
int main() {
    NetworkTablesClient client;

    std::cout << "Connecting to Network Tables at ws://127.0.0.1:5810..." << std::endl;

    // Connect to the local Network Tables server
    if (client.connect("ws://127.0.0.1:5810")) {
        // Subscribe to a few example values
        client.subscribe("SmartDashboard", "autonomous/selected");
        client.subscribe("FMSInfo", "IsRedAlliance");

        // Publish an example value
        client.publish("SmartDashboard", "example_value", "\"Hello from C++ client\"");

        // The client.run() in connect() will block until the connection is closed
    }
    else {
        std::cout << "Failed to connect!" << std::endl;
    }

    return 0;
}
