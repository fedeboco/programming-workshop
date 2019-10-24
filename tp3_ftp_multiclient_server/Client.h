#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <string>
#include <thread>
#include "Protocol.h"
#define ID_LIST_BEGIN "150"
#define ID_LIST_END "226"
#define ID_QUIT "221"
#define CODE_LENGTH 3

/* Individual client. Used by server to make a
 * client list */
class Client {
 private:
    std::thread * thread;

 public:
    int socket_id;
    bool is_alive = true;
    bool logged = false;
    bool quit = false;
    std::string possible_user;

    Client();
    explicit Client(int id);

    int get_id();
    void print_response(const std::string & received_string) const;
    void run_client(int argc, char *argv[]);
    void receive_hello(Protocol & protocol);
    void receive_list(Protocol & protocol);
    void save_thread(std::thread *);
    void shutdown_socket();
    void close_thread();
    static void get_user_input(std::string & user_input);
    bool quit_received(const std::string & response);
    bool is_list_start(const std::string & str);
    bool is_accepted();
    bool is_dead();
    std::string get_code(std::string const &);
};

#endif  // SERVER_CLIENT_H
