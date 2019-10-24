#include <string>
#include "Protocol.h"

/* Sets up host and port for client, port and config for server. */
Protocol::Protocol(int argc, char * argv[], char type) {
    if ( validate_args(argc, argv, type) ) {
        switch (type) {
            case 'S':
                port = argv[1];
                config_file = argv[2];
                break;
            case 'C':
                hostname = argv[1];
                port = argv[2];
                break;
        }
    }
}

/* Sends entire string at once. */
void Protocol::send_string(const std::string &cmd, int socket_id) {
    socket.send(cmd.length(), const_cast<char *>(cmd.c_str()), socket_id);
}

/* Receives string one byte at a time.
 * String delimited by RECEIVED_STRING_DELIMITER. */
std::string Protocol::receive_string(int id) {
    std::string str;
    char aux = 0;

    while (aux != RECEIVED_STRING_DELIMITER) {
        socket.receive(1, &aux, id);
        str.push_back(aux);
    }
    return str;
}

/* Validates client execution arguments. */
bool Protocol::validate_client_args(int argc, char **argv) const {
    return argv != nullptr && argc == 3;
}

/* Validates server execution arguments. */
bool Protocol::validate_server_args(int argc, char **argv) const {
    return argv != nullptr && argc == 3;
}

/* Validates client or server arguments. */
bool Protocol::validate_args(int argc, char **argv, char type) const {
    switch (type) {
        case 'C':
            return validate_client_args(argc, argv);
        case 'S':
            return validate_server_args(argc, argv);
        default:
            return false;
    }
}
