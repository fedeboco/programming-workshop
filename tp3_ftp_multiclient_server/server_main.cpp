#include <thread>
#include "Protocol.h"
#include "Config.h"
#include "common_server.h"

int main(int argc, char * argv[]) {
    Protocol ftp(argc, argv, 'S');
    try {
        Server server(argv[1], argv[2]);
        Config configuration(argv[2]);
        std::thread server_acceptor = std::thread(&Server::run_server,
                                                  &server,
                                                  std::ref(configuration),
                                                  std::ref(server),
                                                  std::ref(ftp));
        server.quit();
        server.close_all_sockets();
        server_acceptor.join();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
