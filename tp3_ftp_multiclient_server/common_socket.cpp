#define _POSIX_C_SOURCE 200112L

#include <iostream>
#include "Socket.h"

/* Constructor: not connected socket */
Socket::Socket() {
    socket_id = -1;
}

/* Constructor: based on connected socket */
Socket::Socket(int id) {
    socket_id = id;
    is_alive = true;
}

/* Sends len bytes of buf using the socket
 * identified by socket_id */
void Socket::send(int len, char *buf, int socket_id) {
    bool socket_running = true;
    int bytes_sent = 0;
    int st = 0;

    while (socket_running && bytes_sent < len) {
        st = ::send(socket_id, &buf[bytes_sent], len - bytes_sent, 0);
        if (st == 0) {
            socket_running = false;
            is_alive = false;
        } else if (st == -1) {
            socket_running = false;

        } else {
            bytes_sent = bytes_sent + st;
        }
    }
    if (st == -1) {
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_SENDING,
                    "socket_id = ",
                    socket_id,
                    errno);
    }
}

/* Receives len bytes of buf using the socket
 * identified by socket */
int Socket::receive(int len, char *buf, int socket) {
    bool socket_running = true;
    int bytes_received = 0;
    int st = 0;
    while (socket_running && bytes_received < len) {
        st = recv(socket, &buf[bytes_received],
                  len - bytes_received,
                  MSG_NOSIGNAL);
        if (st == 0) {
            socket_running = false;
        } else if (st == -1) {
            socket_running = false;
        } else {
            bytes_received = bytes_received + st;
        }
    }
    if ( bytes_received > 0 ) {
        return bytes_received;
    } else if (bytes_received == 0) {
        is_alive = false;
        return -1;
    } else {
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_RECEPTION,
                    "socket_id = ",
                    socket_id,
                    errno);
    }
}

/* Constructor: creates Socket to work as client */
Socket::Socket(char *hostname, char *port) {
    int st;
    struct addrinfo hints {};
    struct addrinfo *res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;

    st = getaddrinfo(hostname, port, &hints, &res);
    if (st != 0)
        throw Error("%s (%s%d).",
                    ERROR_ADDRINFO_SERVER,
                    "st = ",
                    st);

    socket_id = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_id == -1) {
        freeaddrinfo(res);
        throw Error("%s (%s%d).",
                    ERROR_SOCKET_ENDPOINT,
                    "socket_id = ",
                    socket_id);
    }

    st = ::connect(socket_id, res->ai_addr, res->ai_addrlen);
    if (st == -1) {
        freeaddrinfo(res);
        close(socket_id);
        throw Error("%s (%s%d).",
                    ERROR_CONNECTING,
                    "st = ",
                    st);
    }

    freeaddrinfo(res);
    if (st !=0 || socket_id == -1)
        throw Error("%s (%s%d, %s%d).",
                    ERROR_WAKING_SOCKET,
                    "st = ",
                    st,
                    "socket_id = ",
                    socket_id);
    is_alive = true;
}

/* Constructor: creates Socket to work as server */
Socket::Socket(char *port) {
    int c = 1;
    int st;
    struct addrinfo hints {};
    struct addrinfo *res;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    st = getaddrinfo(nullptr, port, &hints, &res);
    if (st != 0)
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_ADDRINFO_SERVER,
                    "st = ",
                    st,
                    errno);

    socket_id = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_id == -1) {
        freeaddrinfo(res);
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_SOCKET_ENDPOINT,
                    "socket_id = ",
                    socket_id,
                    errno);
    }

    st = setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &c, sizeof(&c));
    if (st == -1) {
        freeaddrinfo(res);
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_CONFIGURING_CLIENT_SOCKET,
                    "st = ",
                    st,
                    errno);
    }
    bind(res);
    listen();
    freeaddrinfo(res);
    is_alive = true;
}

/* Sets server socket to listen incoming connections. */
void Socket::listen() {
    if (::listen(socket_id, WAITING_CLIENTS) == -1) {
        close(socket_id);
        throw Error("%s Error number: %d.",
                    ERROR_SETTING_LISTEN,
                    errno);
    }
}

/* Assigns address specified by addr to the socket */
void Socket::bind(addrinfo_t *res) {
    if (::bind(socket_id, res->ai_addr, res->ai_addrlen) == -1) {
        close(socket_id);
        throw Error("%s Error number: %d.",
                    ERROR_BINDING,
                    errno);
    }
}

/* Accepts incoming connection from client to server */
int Socket::accept() {
    int peer_id;
    peer_id = ::accept(socket_id, nullptr, nullptr);
    if (peer_id == -1) {
        close(socket_id);
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_SOCKET_ACCEPTOR,
                    "socket_id = ",
                    socket_id,
                    errno);
    }
    return peer_id;
}

/* Sends shutdown signal and closes the socket */
void Socket::shutdown() {
    if (::shutdown(socket_id, SHUT_RDWR) == -1) {
        throw Error("%s (%s%d). Error number: %d.",
                    ERROR_SHUTDOWN,
                    "socket_id = ",
                    socket_id,
                    errno);
    }
    close(socket_id);
}

/* Checks if socket has been accepted */
bool Socket::is_accepted() { return socket_id != -1; }

/* Gets socket identification number */
int Socket::get_id() {return socket_id;}

/* Checks if socket is ON or OFF */
bool Socket::is_dead() {return !is_alive;}

/* Prints socket's identification number */
std::ostream& Socket::operator<<(std::ostream& os) const {
    os << socket_id;
    return os;
}
