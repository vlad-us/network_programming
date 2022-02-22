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

namespace fs = boost::filesystem;

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
    ssize_t bytes_sent = write(m_socket, _data, data.length());
    // ssize_t bytes_sent = send(m_socket, _data, data.length(), MSG_NOSIGNAL);
    std::cout << "bytes_sent: " << bytes_sent << '\n';
    if( bytes_sent < 0 )
    {
        std::cout << "error: " << this->error_to_string(errno) << '\n';
    }
    return bytes_sent;
}

plu_header tcp_client::get_plu_header(int32_t file_number)
{
    plu_header header;
    int bytes_received = read(m_socket, &header, sizeof(header));
    header.plu_record_size = ntohs(header.plu_record_size);
    header.plu_number = fromBcd(ntohl(header.plu_number));
    std::cout << "get_plu_header::bytes_received: " << bytes_received << '\n';
    return header;
}

std::string tcp_client::read_from_socket(int bufer_size)
{
    char buf[bufer_size];
    size_t bytes_received = read(m_socket, buf, sizeof(buf));
    std::cout << "read_from_socket::bytes_received: " << bytes_received << '\n';
    std::string response{ buf, bytes_received };
    return response;
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

    std::stringstream command = prepare_command(query_type::read, file_types::plu, 0);
    client.send_to_socket(command.str());
    plu_header plu_header = client.get_plu_header();
    std::string plu_record = client.read_from_socket(plu_header.plu_record_size + 1);
    std::cout << "PLU_HEADER\n";
    std::cout << "plu_header.plu_number: " << plu_header.plu_number << '\n';
    std::cout << "plu_header.plu_record_size: " << plu_header.plu_record_size << '\n';
    std::cout << "PLU_RECORD\n";
    std::cout << "plu_record.length(): " << plu_record.length() << '\n';
    std::ofstream o_file{ "digi_output", std::ios_base::binary };
    o_file << plu_header.plu_number << plu_header.plu_record_size << plu_record;
    o_file << "\n\n";
    std::cout << "===\n";
    command = prepare_command(query_type::read, file_types::plu, htonl(toBcd(plu_header.plu_number)));  // htonl(toBcd(plu_header.plu_number))
    client.send_to_socket(command.str());
    plu_header = client.get_plu_header();
    plu_record = client.read_from_socket(plu_header.plu_record_size + 1);
    std::cout << "PLU_HEADER\n";
    std::cout << "plu_header.plu_number: " << plu_header.plu_number << '\n';
    std::cout << "plu_header.plu_record_size: " << plu_header.plu_record_size << '\n';
    std::cout << "PLU_RECORD\n";
    std::cout << "plu_record.length(): " << plu_record.length() << '\n';
    o_file << plu_header.plu_number << plu_header.plu_record_size << plu_record;
    o_file << "\n\n";
    std::cout << "===\n";
    command = prepare_command(query_type::read, file_types::plu, htonl(toBcd(plu_header.plu_number)));  // htonl(toBcd(plu_header.plu_number))
    client.send_to_socket(command.str());
    plu_header = client.get_plu_header();
    plu_record = client.read_from_socket(plu_header.plu_record_size + 1);
    std::cout << "PLU_HEADER\n";
    std::cout << "plu_header.plu_number: " << plu_header.plu_number << '\n';
    std::cout << "plu_header.plu_record_size: " << plu_header.plu_record_size << '\n';
    std::cout << "PLU_RECORD\n";
    std::cout << "plu_record.length(): " << plu_record.length() << '\n';
    o_file << plu_header.plu_number << plu_header.plu_record_size << plu_record;
    o_file << "\n\n";

    // o_file << plu_header.plu_number << plu_header.plu_record_size << plu_record;
    // o_file.write(plu_record.c_str(), sizeof(*(plu_record.c_str())) * plu_record.length());
    o_file.close();
    return true;
}

std::stringstream prepare_command(query_type action, file_types file_type, int note_num)
{
    std::stringstream ss;
    ss.write(reinterpret_cast<char*>(&action), sizeof(action));
    ss.write(reinterpret_cast<char*>(&file_type), sizeof(file_type));
    ss.write(reinterpret_cast<char*>(&note_num), sizeof(note_num));
    return ss;
}


int main(int argc, char* argv[])
{
    bool success = false;
    success = run(argc, argv);
    return success ? 0 : 1;
}