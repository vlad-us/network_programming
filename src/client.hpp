#pragma once
// std
#include <string>
// socket api
#include <sys/socket.h>
// boost
#include <boost/program_options.hpp>


namespace po = boost::program_options;

class tcp_client
{
public:
    tcp_client();
    ~tcp_client();
private:
    std::string m_host;
    int m_port;
    sockaddr* m_addr;
    int m_socket;
public:
    /// \todo getter fot socket
    bool open_socket();
    bool set_addr(std::string host, int port);
    bool create_connection();
    int send_to_socket(const std::string& data);
    int send_to_socket(char data[], int size);
    std::string read_from_socket();
    void close_socket();
    static std::string error_to_string(int error_code);
    static po::variables_map read_program_options(int argc, char* argv[]);
};