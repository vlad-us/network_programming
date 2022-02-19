// Socket API
#include <sys/socket.h>
#include <netinet/in.h>  // htons()
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>  // errno
#include <string.h> // memset()
#include <unistd.h>  // write()
#include <sys/resource.h>
#include <sys/time.h>
// std
#include <iostream>
#include <string>

#define MAX_READ_BUF 4096  /* максимальный размер буфера при чтении 4 КБ */


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
    void set_addr(std::string host, int port);
    bool create_connection();
    int send_to_socket(const std::string& data);
    int read_from_socket();
    void close_socket();
    static std::string error_to_string(int error_code);
};

tcp_client::tcp_client()
{}

tcp_client::~tcp_client()
{}

bool tcp_client::open_socket()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    return m_socket > 0 ? true : false;
}

void tcp_client::set_addr(std::string host, int port)
{
    sockaddr_in temp_addr_struct;
    memset(&temp_addr_struct, 0, sizeof(temp_addr_struct));
    temp_addr_struct.sin_family = AF_INET; // задать семейство протоколов
    temp_addr_struct.sin_port = htons(port);  // задать порт
    int host_convert_success = inet_pton(PF_INET, host.c_str(), &temp_addr_struct.sin_addr);  // задать хост
    if( !host_convert_success )
    {
        if( host_convert_success == 0 )
        {
            throw std::runtime_error("Invalid host address: " + static_cast<std::string>(host));
        }
        if( host_convert_success < 0 )
        {
            throw std::invalid_argument("The implementation does not support the specified address family");
        }
    }
    m_addr = reinterpret_cast<sockaddr*>(&temp_addr_struct);
}

bool tcp_client::create_connection()
{
    int connect_success = connect(m_socket, m_addr, sizeof(*m_addr));
    return connect_success == 0 ? true : false;
}

int tcp_client::send_to_socket(const std::string& data)
{
    const char* _data = data.c_str();
    int bytes_sent = write(m_socket, _data, data.length());
    return bytes_sent;
}

int tcp_client::read_from_socket()
{
    char buf[MAX_READ_BUF + 1];
    int bytes_read = 0;
    std::cout << "response: ";
    while( (bytes_read = read(m_socket, buf, MAX_READ_BUF)) > 0 )
    {
        buf[bytes_read] = 0;
        std::cout << buf;
    }
    std::cout << '\n';
    return 0;
}

void tcp_client::close_socket()
{
    close(m_socket);
}

std::string tcp_client::error_to_string(int error_code)
{
    switch( error_code )
    {
        case EPROTONOSUPPORT: return "Protocol not supported";
        case ENFILE: return "Not enough memory to create a new socket";
        case EMFILE: return "Process file table overflow";
        case EACCES: return "No access to create a socket of the specified type and/or protocol";
        case ENOBUFS: case ENOMEM: return "Not enough memory to create socket";
        case EINVAL: return "Unknown protocol or protocol set not available";
        case EBADF: return "File descriptor in bad state";
        case EFAULT: return "<buf> points out of the available address space";
        case ENOTSOCK: return "Socket operation on non-socket";
        case EISCONN: return "Transport endpoint is already connected";
        case ECONNREFUSED: return "Connection refused";
        case ETIMEDOUT: return "Connection timed out";
        case ENETUNREACH: return "Network is unreachable";
        case EADDRINUSE: return "Address already in use";
        case EINPROGRESS: return "Operation now in progress";
        case EALREADY: return "Operation already in progress";
        case EAGAIN: return "Operation would block";
        case EAFNOSUPPORT: return "Address family not supported by protocol";
        case EPERM: return "The user tried to connect to a broadcast address without setting the broadcast flag on the socket, or the connection request failed due to a local firewall rule";
        case EFBIG: return "An attempt was made to write to a file that exceeds the maximum size limit defined by the system or process, or an attempt was made to write data in a file to a position outside the maximum allowable offset ";
        default: return "Unknown error";
    }
}


int main()
{
    tcp_client client;
    bool socket_opened = client.open_socket();
    if( socket_opened )
    {
        client.set_addr("127.0.0.1", 8088);
        bool connect_success = client.create_connection();
        if( connect_success )
        {
            int bytes_sent = client.send_to_socket("Consequat fugiat ex reprehenderit proident sunt. Pariatur veniam consequat labore ea aliquip sit ut anim minim occaecat. Excepteur nulla labore sit cupidatat ea.");
            int bytes_received = client.read_from_socket();
            client.close_socket();
        }
        else
        {
            std::cout << "Error: " << tcp_client::error_to_string(errno) << '\n';
        }
    }

    // int sock = socket(AF_INET, SOCK_STREAM, 0);
    // sockaddr_in addr;
    // const char* host = "127.0.0.1";
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(8088);
    // inet_aton(host, &addr.sin_addr);
    // int connect_success = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    // if (connect_success == 0)
    // {
    //     std::cout << "connection success\n";
    //     char command[] = "hello"; //"\r\nECS\tVER\t100\t\r\nUPL\tTIM\t\r\nEND\tTIM\t\r\nEND\tECS\t\r\n";
    //     std::string response = "";
    //     char buf[1] = "";
    //     int bytes_sent = send(sock, command, sizeof(command), 0);
    //     int bytes_received = recv(sock, &(buf[0]), sizeof(buf) - 1, 0); ///
    //     response += buf;
    //     std::cout << "response: " << response << '\n';
    //     std::cout << "response size: " << sizeof(response) << '\n';
    //     close(sock);
    // }
    // else
    // {
    //     std::cout << "error\n";
    // }

    // rlimit r;
    // int limit = getrlimit(RLIMIT_FSIZE, &r);
    // std::cout << r.rlim_cur << '\n';
    // std::cout << r.rlim_max << '\n';
    
    return 0;
}