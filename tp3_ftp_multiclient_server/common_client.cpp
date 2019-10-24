#include <string>
#include "Client.h"

/* Constructor: disconnected client */
Client::Client() {
    socket_id = -1;
}

/* Constructor: based on existing connection */
Client::Client(int id) {
    socket_id = id;
    if (id > 0)
        is_alive = true;
}

/* Saves thread's address used to attend
 * client's requests on server side. Used
 * for later closure */
void Client::save_thread(std::thread * th) {
    thread = th;
}

/* Closes thread used to attend the client
 * in the server */
void Client::close_thread() {
    if (thread->joinable())
        thread->join();
}

/* Sends shutdown signal and closes the
 * socket related to that client */
void Client::shutdown_socket() {
    ::shutdown(socket_id, SHUT_RDWR);
    close(socket_id);
}

/* Checks if client is connected */
bool Client::is_dead() {
    return !is_alive;
}

/* Implements the logic to send requests to the
 * server and receives proper responses. The server
 * checks if the requests are valid commands */
void Client::run_client(int argc, char * argv[]) {
    try {
        Socket socket = Socket(argv[1], argv[2]);
        socket_id = socket.get_id();
        std::string user_input, response;
        Protocol ftp(argc, argv, 'C');
        bool running = true;

        receive_hello(ftp);
        while (running) {
            get_user_input(user_input);
            if (user_input != "\n") {
                ftp.send_string(user_input, socket.get_id());
                response = ftp.receive_string(socket.get_id());
                print_response(response);
                if (is_list_start(response))
                    receive_list(ftp);
            } else if (user_input == "\n" || quit_received(response)) {
                ftp.send_string(user_input, socket.get_id());
                running = false;
            }
        }
        socket.shutdown();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

/* Checks if server accepted the quit signal */
bool Client::quit_received(const std::string & response) {
    return get_code(response) == std::string(ID_QUIT);
}

/* Waits until reception of initial message from server */
void Client::receive_hello(Protocol & protocol) {
    std::string response;
    response = protocol.receive_string(socket_id);
    print_response(response);
}

/* Reads user's query from input */
void Client::get_user_input(std::string & user_input) {
    user_input.clear();
    getline(std::cin, user_input);
    user_input.push_back('\n');
}

/* Checks if the server started to send a list of directories */
bool Client::is_list_start(const std::string & str) {
    return get_code(str) == std::string(ID_LIST_BEGIN);
}

/* Prints code and message sent by server */
void Client::print_response(const std::string & received_string) const {
    std::cout << received_string;
}

/* Receives list of directories and prints it */
void Client::receive_list(Protocol & protocol) {
    std::string list_item;
    while (get_code(list_item) != ID_LIST_END) {
        list_item = protocol.receive_string(socket_id);
        print_response(list_item);
    }
}

/* Gets response's code from the string*/
std::string Client::get_code(std::string const & received_string) {
    std::string code;
    for (int i=0; i < CODE_LENGTH; i++)
        code.push_back(received_string[i]);
    return code;
}

/* Checks if client has been accepted by server */
bool Client::is_accepted() { return socket_id != -1; }

/* Gets socket's identification number of a client*/
int Client::get_id() { return socket_id; }

