#include <string>
#include <vector>
#include <set>
#include "common_server.h"
#include "Command.h"

/* Implements the acceptor thread. Constantly waits for incoming clients.
 * If a client is accepted, launches a thread for that client and checks
 * if other clients ceased operation closing their threads and sockets */
void Server::run_server(Config &cfg, Server &server, Protocol &protocol) {
    int socket_id = -1;
    std::vector<std::thread> threads(200);

    try {
        socket = Socket(port);
        while (is_running) {
            socket_id = socket.accept();
            Client client(socket_id);
            if (client.is_accepted()) {
                threads.emplace_back(std::thread(&Server::respond_client,
                                     this,
                                     std::ref(cfg),
                                     socket_id,
                                     std::ref(protocol)));
                client.save_thread(&threads.back());
                clients.push_back(client);
                close_dead_sockets();
            }
        }
        close_all_sockets();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        close_all_sockets();
    }
}

/* Implements individual client's thread. Receives strings from the client
 * and processes them to determine if they constitute a valid command. If so,
 * answers properly sending the required information, updates directories
 * or verifies the login credentials */
void Server::respond_client(Config &cfg, int socket_id, Protocol &protocol) {
    std::string client_query;
    Client cli(socket_id);
    try {
        send_hello(cli, cfg, protocol);
        while (!cli.quit) {
            client_query = protocol.receive_string(cli.get_id());
            if (client_query.length() > 1) {
                respond_query(client_query, cfg, cli, &protocol);
            } else {
                cli.quit = true;
            }
        }
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

/* Creates a command to give the client a response. Updates directories
 * or credentials if necessary */
void Server::respond_query(std::string &client_query,
                           Config &cfg,
                           Client &cli,
                           Protocol *protocol) {
    Command * cmd;
    CommandCreator creator;
    cmd = creator.create_command(client_query);
    cmd->send_response(cfg, protocol, this, cli);
    delete cmd;
}

/* Sends initial message to the client to start communication */
void Server::send_hello(const Client& cli, Config &cfg, Protocol &protocol) {
    protocol.send_string(std::string(ID_NEW_CLIENT) +
                " " +
                cfg.get_message(MSG_NEW_CLIENT),
                cli.socket_id);
}

/* Closes all sockets if quit signal has been received */
void Server::close_all_sockets() {
    std::lock_guard<std::mutex> lock(m);
    while (!clients.empty()) {
        clients.back().shutdown_socket();
        clients.back().close_thread();
        clients.pop_back();
    }
}

/* Closes dead sockets if the socket ceased the connection */
void Server::close_dead_sockets() {
    std::lock_guard<std::mutex> lock(m);
    for (client_it cli = clients.begin();
         cli != clients.end();
         cli++) {
        if (cli->is_dead()) {
            cli->close_thread();
            cli->shutdown_socket();
            clients.erase(cli);
        }
    }
}

/* Thread-safe. If absent, adds the directory requested by client */
bool Server::add_directory(const std::string& directory) {
    std::lock_guard<std::mutex> lock(dir_m);
    if (directories.find(directory) == directories.end()) {
        directories.insert(directory);
        return true;
    }
    return false;
}

/* Thread-safe. If present, adds the directory requested by client */
bool Server::remove_directory(const std::string & directory) {
    std::lock_guard<std::mutex> lock(dir_m);
    std::set<std::string>::iterator it;
    it = directories.find(directory);
    if (it != directories.end()) {
        directories.erase(it);
        return true;
    }
    return false;
}

/* Thread-safe. Gets a formatted list of all directories for the client */
std::string Server::get_list() {
    std::lock_guard<std::mutex> lock(dir_m);
    std::string list;
    dir_it it;
    for (it=directories.begin(); it != directories.end(); it++)
        list += DIR_FORMAT + *it;
    return list;
}

/* Gets the directory list's size */
int Server::directories_size() {
    std::lock_guard<std::mutex> lock(dir_m);
    return directories.size();
}

/* Waits upon the user inputs a quit command for server shutdown*/
void Server::quit() {
    std::string str;
    while (running()) {
        getline(std::cin, str);
        if ( str == SERVER_QUIT_CMD1 || str == SERVER_QUIT_CMD2 )
            stop();
    }
}

/* Checks if server is running */
bool Server::running() const {return is_running;}

/* If server's quit command has been received,
 * closes all the sockets, producing the acceptor
 * thread to close all threads. Also closes server's
 * socket.*/
void Server::stop() {
    close_all_sockets();
    is_running = false;
    socket.shutdown();
}

/* Constructor: creates server in specified
 * port and configuration file */
Server::Server(char *h, char *c) {
    port = h;
    config = c;
}
