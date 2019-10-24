#ifndef SERVER_COMMON_SERVER_H
#define SERVER_COMMON_SERVER_H

#include <vector>
#include <string>
#include <set>
#include "Client.h"

typedef std::vector<Client>::iterator client_it;
typedef std::set<std::string>::iterator dir_it;

/* Implements the logic necessary to process
 * clients connections and saves the shared list
 * of directories. Stores a list of clients. */
class Server {
 private:
    std::vector<Client> clients;
    std::set<std::string> directories;
    Socket socket;
    bool is_running = true;
    std::mutex m;
    std::mutex dir_m;
    char * port;
    char * config;

 public:
    Server(char *string, char *string1);

    static void send_hello(const Client& cli, Config &cfg, Protocol &protocol);
    void respond_query(std::string &client_query,
                       Config &cfg,
                       Client &cli,
                       Protocol *protocol);
    void respond_client(Config &cfg, int socket_id, Protocol &protocol);
    void run_server(Config &cfg, Server &server, Protocol &protocol);
    void close_dead_sockets();
    void close_all_sockets();
    void stop();
    void quit();
    bool remove_directory(const std::string &directory);
    bool add_directory(const std::string&);
    bool running() const;
    int directories_size();
    std::string get_list();
};


#endif  // SERVER_COMMON_SERVER_H
