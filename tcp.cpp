#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tcp.hpp"

namespace dilichev
{

namespace tcp
{

Connection::Connection() : socket(-1)
{
}

Connection::Connection(const Connection &rhs) : socket(rhs.socket)
{
    socketsCount[socket] += 1;
}

Connection::Connection(Connection &&rhs) : socket(rhs.socket)
{
    rhs.socket = -1;
}

Connection::~Connection()
{
    close();
}

ssize_t Connection::send(const void *data, std::size_t size)
{
    if(socket == -1)
    {
        throw -1;
    }

    return ::send(socket, data, size, 0);
}

Connection::Data Connection::recv(std::size_t size)
{
    if(socket == -1)
    {
        throw -1;
    }

    Connection::Data buffer(size, 0);
    ssize_t l = ::recv(socket, buffer.data(), size, 0);
    if(l < 0)
    {
        return Connection::Data();
    }
    return Connection::Data(buffer.begin(), buffer.begin() + l);
}

void Connection::connect(const std::string &ip, unsigned short port)
{
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0)
    {
        throw -1;
    }
    socketsCount[socket] += 1;

    sockaddr_in addr;
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    if(inet_aton(ip.c_str(), &addr.sin_addr) < 0)
    {
        throw -2;
    }

    if(::connect(socket, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        throw -3;
    }
}

void Connection::close()
{
    if(socket != -1)
    {
        if((socketsCount[socket] -= 1) == 0)
        {
            ::close(socket);
        }
        socket = -1;
    }
}

}

}