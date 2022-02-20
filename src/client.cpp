// local
#include "client.hpp"
// socket api
#include <netinet/in.h> // htons()
#include <arpa/inet.h>  // inet_pton()
#include <string.h>     // memset()
#include <unistd.h>     // write() read()
// std
#include <iostream>

#define MAX_READ_BUF 4096  /* максимальный размер буфера при чтении 4 КБ */

tcp_client::tcp_client()
{}

tcp_client::~tcp_client()
{}

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
    int host_convert_success = inet_pton(PF_INET, host.c_str(), &temp_addr_struct);  // задать хост
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
    std::string error_text{ strerror(error_code) };
    return error_text;
}


int main()
{
    tcp_client client;
    bool socket_opened = client.open_socket();
    if( socket_opened )
    {
        client.set_addr("127.0.0.a", 8088);
        bool connect_success = client.create_connection();
        if( connect_success )
        {
            int bytes_sent = client.send_to_socket("Dolor qui non ipsum anim pariatur tempor consectetur consectetur commodo officia irure proident elit. Est quis occaecat sint esse reprehenderit eu mollit voluptate dolor do dolor. Commodo duis consectetur cupidatat consequat sint anim incididunt dolor exercitation minim laboris. Et commodo qui dolor fugiat sit deserunt sunt laboris id laborum ullamco non.Incididunt qui nisi incididunt Lorem ut sunt est do. Lorem velit amet ut culpa. Cillum duis eu voluptate nostrud nostrud ut eu deserunt nulla eiusmod. Ea elit nulla Lorem mollit consectetur aute.Consequat eu incididunt ut nisi Lorem tempor qui et esse consequat cupidatat. Ex laborum est ullamco magna esse eiusmod nisi sint cupidatat do aliqua. Ad sint veniam ad eu. Ipsum eiusmod ut aliqua irure exercitation esse. Voluptate officia consectetur incididunt amet minim laboris ad commodo nisi anim non ad officia quis. Consequat duis sit fugiat veniam velit. Sit ipsum culpa est consequat labore et sit anim duis quis.Veniam amet incididunt esse eu deserunt tempor ea eiusmod occaecat consectetur ullamco anim officia. Eiusmod nulla laboris culpa tempor culpa incididunt duis dolor sit magna elit culpa voluptate. Commodo consequat id mollit sunt in ipsum. Consectetur esse commodo deserunt ut ipsum aliquip id.Qui anim occaecat labore sint excepteur fugiat amet. Sit minim ullamco aliqua fugiat et irure sit nostrud. Lorem eiusmod qui excepteur ad in et cillum dolore adipisicing. Nulla proident aliquip esse ullamco laboris dolor commodo aliquip aliquip proident quis consequat. Ipsum pariatur irure enim adipisicing nisi cillum mollit mollit pariatur incididunt. Labore ullamco mollit esse reprehenderit esse ea. Culpa adipisicing occaecat amet eu aliquip culpa nulla enim id et sunt ut elit.Qui in quis in ea pariatur. Eiusmod eiusmod ullamco sunt ex mollit nulla. Labore irure pariatur laborum et et aute esse aliquip nulla proident.Labore nisi sunt officia qui sit et ipsum. Exercitation et anim sunt sint pariatur Lorem cillum. Tempor nulla cupidatat nisi aliquip elit in id magna dolore.");
            int bytes_received = client.read_from_socket();
            client.close_socket();
        }
        else
        {
            std::cout << "Error: " << tcp_client::error_to_string(errno) << '\n';
        }
    }
    else
    {
        std::cout << "Error: " << tcp_client::error_to_string(errno) << '\n';
    }
    // std::cout << std::boolalpha << bool(0) << '\n';
    // http://ru.archive.ubuntu.com/ubuntu/pool/main/libx/libxcb/libxcb-xinerama0_1.14-2_amd64.deb
}