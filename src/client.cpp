// local
#include "client.hpp"
// socket api
#include <netinet/in.h> // htons()
#include <arpa/inet.h>  // inet_pton()
#include <string.h>     // memset()
#include <unistd.h>     // write() read()
// std
#include <iostream>
#include <bitset>
// boost
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#define MAX_READ_BUF 128  /* максимальный размер буфера при чтении 4 КБ */

namespace fs = boost::filesystem;

std::stringstream prepare_command(int note_num);

tcp_client::tcp_client()
{}

tcp_client::~tcp_client()
{
    ::close(m_socket);
}

bool tcp_client::open_socket()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    return m_socket > 0 ? true : false;
}

bool tcp_client::set_addr(std::string host, int port)
{
    sockaddr_in temp_addr_struct;
    memset(&temp_addr_struct, 0, sizeof(temp_addr_struct));
    temp_addr_struct.sin_family = AF_INET; // задать семейство протоколов
    temp_addr_struct.sin_port = htons(port);  // задать порт
    int host_convert_success = inet_pton(PF_INET, host.c_str(), &temp_addr_struct.sin_addr);  // задать хост
    if( !host_convert_success )
    {
        return false;  /* переменная errno не устанавливается */
    }
    m_addr = reinterpret_cast<sockaddr*>(&temp_addr_struct);
    return true;
}

bool tcp_client::create_connection()
{
    int connect_success = connect(m_socket, m_addr, sizeof(*m_addr));
    return connect_success == 0 ? true : false;
}

int tcp_client::send_to_socket(const std::string& data)
{
    const char* _data = data.c_str();
    // std::cout << "data to send: " << data.length() << '\n';
    int bytes_sent = write(m_socket, _data, data.length());
    // std::cout << "bytes_sent: " << bytes_sent << '\n';
    return bytes_sent;
}

int tcp_client::send_to_socket(char data[], int size)
{
    int bytes_sent = write(m_socket, data, size);
    std::cout << "bytes_to_send: " << size << '\n';
    std::cout << "bytes_sent: " << bytes_sent << '\n';
    return bytes_sent;
}

std::string tcp_client::read_from_socket()
{
    std::stringstream response;
    char buf[MAX_READ_BUF + 1];
    int bytes_read = 0;
    while( (bytes_read = read(m_socket, buf, MAX_READ_BUF)) > 0 )
    {
        buf[bytes_read] = 0;
        response << buf;
        std::cout << "BUF: " << buf << std::endl;
    }
    return response.str();
}

void tcp_client::close_socket()
{
    close(m_socket);
}

std::string tcp_client::error_to_string(int error_code)
{
    std::string error_text{ strerror(error_code) };
    return error_text;
}

po::variables_map tcp_client::read_program_options(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("ip", po::value<std::string>() -> required(), "IP address")
        ("port", po::value<int>() -> required(), "PORT")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    return vm;
}

bool run(int argc, char* argv[])
{
    po::variables_map vm = tcp_client::read_program_options(argc, argv);
    tcp_client client;
    bool socked_opened = client.open_socket();
    if( !socked_opened )
    {
        std::cerr << "Error: " << tcp_client::error_to_string(errno) << '\n';
        return false;
    }
    client.set_addr(vm["ip"].as<std::string>(), vm["port"].as<int>());
    bool connect_success = client.create_connection();
    if( !connect_success )
    {
        std::cerr << "Error: " << tcp_client::error_to_string(errno) << '\n';
        return false;
    }
    char data_to_send[6];
    // std::ifstream file{ "/home/us_va/cpp_learn/network_programming/outbut", std::ios_base::binary };
    // file.read(data_to_send, sizeof(data_to_send));
    std::stringstream ss = prepare_command(0x00000000);
    ss.read(data_to_send, sizeof(data_to_send));
    client.send_to_socket(data_to_send, sizeof(data_to_send));
    // int bytes_to_send = sizeof(*(data_to_send.c_str())) * data_to_send.length();
    // int bytes_sent = client.send_to_socket(data_to_send);
    // if( bytes_to_send != bytes_sent )
    // {
    //     std::cerr << "Error: ошибка при отправке данных\n";
    //     return false;
    // }
    // std::stringstream ss;
    std::string response = client.read_from_socket();
    // ss << response;
    std::cout << "response: " << response.c_str() << '\n';
    std::cout << "response size: " << sizeof(*(response.c_str())) * response.length() << '\n';
    std::ofstream o_file{ "digi_output", std::ios_base::binary };
    o_file.write(response.c_str(), sizeof(*(response.c_str())) * response.length());
    o_file.close();
    return true;
}

std::stringstream prepare_command(int note_num)
{
    std::stringstream ss;
    char command_code = 0xF7;
    char file_num = 0x25;
    ss.write(reinterpret_cast<char*>(&command_code), sizeof(command_code));
    ss.write(reinterpret_cast<char*>(&file_num), sizeof(file_num));
    ss.write(reinterpret_cast<char*>(&note_num), sizeof(note_num));
    return ss;
}


int main(int argc, char* argv[])
{
    // std::ofstream file{ "/home/us_va/cpp_learn/network_programming/file_26H", std::ios_base::binary };

    bool success = false;
    success = run(argc, argv);
    return success ? 0 : 1;
}