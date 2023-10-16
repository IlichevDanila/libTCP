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

Socket::Socket(int sock) : socket(sock), state(State::CLOSED)
{
    if(socket != -1)
    {
        socketsCount[socket] += 1;
    }
}

Socket::Socket(const Socket &rhs) : socket(rhs.socket), state(rhs.state)
{
    if(socket != -1)
    {
        socketsCount[socket] += 1;
    }
}

Socket::Socket(Socket &&rhs) : socket(rhs.socket), state(rhs.state)
{
    rhs.socket = -1;
    rhs.state = State::CLOSED;
}

Socket::~Socket()
{
    close();
}

void Socket::listen(unsigned short port)
{
    if(state != State::CLOSED)
    {
        throw -1;
    }

    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0)
    {
        throw -2;
    }
    socketsCount[socket] += 1;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        throw -3;
    }

    if(::listen(socket, 5) < 0)
    {
        throw -4;
    }

    state = State::LISTEN;
}

Socket Socket::accept()
{
    if(state != State::LISTEN)
    {
        throw -1;
    }

    int sock = ::accept(socket, nullptr, nullptr);
    if(sock < -1)
    {
        throw -2;
    }

    Socket result(sock);
    result.state = State::CONNECTED;
    return result;
}

void Socket::connect(const std::string ip, unsigned short port)
{
    if(state != State::CLOSED)
    {
        throw -1;
    }

    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0)
    {
        throw -1;
    }
    socketsCount[socket] += 1;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_aton(ip.c_str(), &addr.sin_addr) < 0)
    {
        throw -2;
    }

    if(::connect(socket, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
    {
        throw -3;
    }

    state = State::CONNECTED;
}

void Socket::close()
{
    if(socket != -1)
    {
        ::close(socket);
        socketsCount[socket] -= 1;
    }
    state = State::CLOSED;
}

ssize_t Socket::recv(void *buffer, std::size_t size)
{
    if(state != State::CONNECTED)
    {
        throw -1;
    }

    ssize_t res = ::recv(socket, buffer, size, 0);
    if(res < 0)
    {
        throw -2;
    }

    return res;
}

Data Socket::recv(std::size_t size)
{
    if(state != State::CONNECTED)
    {
        throw -1;
    }

    Data buffer(size, 0);

    ssize_t res = ::recv(socket, buffer.data(), size, 0);
    if(res < 0)
    {
        throw -2;
    }

    return Data(buffer.begin(), buffer.begin() + res);
}

ssize_t Socket::send(const void *buffer, std::size_t size)
{
    if(state != State::CONNECTED)
    {
        throw -1;
    }

    ssize_t res = ::send(socket, buffer, size, 0);
    if(res < 0)
    {
        throw -2;
    }

    return res;
}

ssize_t Socket::send(const Data &buffer)
{
    if(state != State::CONNECTED)
    {
        throw -1;
    }

    ssize_t res = ::send(socket, buffer.data(), buffer.size(), 0);
    if(res < 0)
    {
        throw -2;
    }

    return res;
}

}

}