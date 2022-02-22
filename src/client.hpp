#pragma once
// std
#include <string>
// socket api
#include <sys/socket.h>
// boost
#include <boost/program_options.hpp>
// tander
#include <tander/mpl.hpp>

#define MAX_READ_BUF 128  /* максимальный размер буфера при чтении 4 КБ */


namespace po = boost::program_options;

#pragma pack(push, 2)
struct plu_header
{
    uint32_t plu_number; // convert from bcd
    uint16_t plu_record_size;

};
#pragma pack(pop)

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
    std::string read_from_socket(int bufer_size = MAX_READ_BUF + 1);
    void close_socket();
    static std::string error_to_string(int error_code);
    static po::variables_map read_program_options(int argc, char* argv[]);
    plu_header get_plu_header(int32_t file_num = 0);
};

enum class query_type: unsigned char
{
    read = 0xF7
};

enum class file_types: unsigned char
{
    plu = 0x25
};

std::stringstream prepare_command(query_type action, file_types file_type, int note_num);


template <std::size_t From, std::size_t To, class T>
auto switchBase(T t)
-> tander::mpl::EnableIfT<std::is_unsigned<T>::value, T> {
  T result = 0;
  T pow = 1;
  while (t != 0) {
    T rem = t % From;
    t /= From;

    result += rem * pow;
    pow *= To;
  }

  return result;
}

template <class T>
inline T toBcd(T t) { return switchBase<10, 16>(t); }

template <class T>
inline T fromBcd(T t) { return switchBase<16, 10>(t); }

