#ifndef TP3_PROTOCOL_H
#define TP3_PROTOCOL_H

#include <thread>
#include <set>
#include <mutex>
#include <vector>
#include <string>
#include "Socket.h"
#include "Config.h"

#define PROTOCOL_ERR_CMD_CLIENT "Use: ./client <ip/hostname> <port/service>"
#define PROTOCOL_ERR_CMD_SERVER "Use: ./server <port> <config file>"
#define SERVER_QUIT_CMD1 "q"
#define SERVER_QUIT_CMD2 "q\n"
#define RECEIVED_STRING_DELIMITER '\n'

/* Defines transmission methods. Stores address, port
 * and configuration file.*/
class Protocol {
 private:
    class Socket socket;
    char * config_file;
    char * port;
    char * hostname;

 public:
    Protocol(int, char*[], char);

    bool validate_args(int argc, char **argv, char type) const;
    bool validate_server_args(int argc, char **argv) const;
    bool validate_client_args(int argc, char **argv) const;
    void send_string(const std::string &cmd, int socket_id);
    std::string receive_string(int id);
};
#endif  // TP3_PROTOCOL_H
