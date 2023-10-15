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

//Classes' declarations
class Connection;
class ConnectionPool;
class Server;
class Selector;


//Incapsulates socket connections. Substitute for socket
class Connection
{
public:
    using Data = std::vector<std::uint8_t>;

private:
    using SocketsCount = std::unordered_map<int, std::size_t>;

    int socket;
    static SocketsCount socketsCount;

public:
    Connection();
    Connection(const Connection &rhs);
    Connection(Connection &&rhs);
    ~Connection();

    ssize_t send(const void *data, std::size_t size);
    Data recv(std::size_t size);

    void connect(const std::string &ip, unsigned short port);

    void close();
};


//Class for connection set. Substitute for fd_set
class ConnectionPool
{
public:
    using Pool = std::list<Connection>;

private:
    Pool pool;

public:
    ConnectionPool();
    ConnectionPool(const ConnectionPool &rhs);
    ConnectionPool(ConnectionPool &&rhs);
    ~ConnectionPool();

    void add(Connection conn);
    
    Pool::iterator begin();
    Pool::iterator end();
};

//Incapsulates work with "select" function
class Selector
{
private:
    ConnectionPool awaitsRead;//, awaitsWrite, awaitsExcept;
    ConnectionPool readyRead;//, readyWrite, readyExcept;

public:
    Selector();
    Selector(const Selector &rhs);
    Selector(Selector &&rhs);
    ~Selector();

    void addToRead();
    //void addToWrite();
    //void addToExcept();

    void select();

    ConnectionPool getReadyToRead();
    //ConnectionPool getReadyToWrite();
    //ConnectionPool getReadyToExcept();
};

//Class for server socket, obviously
class Server
{
private:
    unsigned short port;
    int socket;

public:
    Server(unsigned short port);
    Server(const Server &rhs) = delete;
    Server(Server &&rhs) = delete;
    ~Server();

    void run();
    
    Connection accept();
};

}

}


#endif