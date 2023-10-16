#ifndef DILICHEV_TCP_HPP
#define DILICHEV_TCP_HPP

//#include <cstddef>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <list>
#include <string>

namespace dilichev
{

namespace tcp
{

using Data = std::vector<std::uint8_t>;

//Classes' declarations
class Socket;
class SocketsPool;
class Selector;


class Socket
{
public:
    enum State {CLOSED, LISTEN, CONNECTED};

private:
    using SocketsCount = std::unordered_map<int, std::size_t>;

    int socket;
    State state;

    static SocketsCount socketsCount;

public:
    Socket(int sock = -1);
    Socket(const Socket &rhs);
    Socket(Socket &&rhs);
    ~Socket();

    void listen(unsigned short port);
    void connect(const std::string ip, unsigned short port);

    Socket accept();

    ssize_t recv(void *buffer, std::size_t size);
    Data recv(std::size_t size);
    ssize_t send(const void *buffer, std::size_t size);
    ssize_t send(const Data &buffer);

    void close();
};


//Class for connection set. Substitute for fd_set
class SocketsPool
{
public:
    using Pool = std::list<Socket>;

private:
    Pool pool;

public:
    SocketsPool();
    SocketsPool(const SocketsPool &rhs);
    SocketsPool(SocketsPool &&rhs);
    ~SocketsPool();

    void add(Socket conn);
    
    Pool::iterator begin();
    Pool::iterator end();
};

//Incapsulates work with "select" function
class Selector
{
private:
    SocketsPool awaitsRead;//, awaitsWrite, awaitsExcept;
    SocketsPool readyRead;//, readyWrite, readyExcept;

public:
    Selector();
    Selector(const Selector &rhs);
    Selector(Selector &&rhs);
    ~Selector();

    void addToRead();
    //void addToWrite();
    //void addToExcept();

    void select();

    SocketsPool getReadyToRead();
    //ConnectionPool getReadyToWrite();
    //ConnectionPool getReadyToExcept();
};

}

}


#endif