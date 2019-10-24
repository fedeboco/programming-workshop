#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include <string>
#include "Client.h"
#include "common_server.h"

#define CMD_USER "USER"
#define CMD_PASS "PASS"
#define CMD_SYST "SYST"
#define CMD_LIST "LIST"
#define CMD_HELP "HELP"
#define CMD_PWD "PWD"
#define CMD_MKD "MKD"
#define CMD_RMD "RMD"
#define CMD_QUIT "QUIT"

#define CODE_LENGTH 3

#define ID_NEW_CLIENT "220"
#define ID_NOT_LOGGED "530"
#define ID_PASS_REQUIRED "331"
#define ID_LOGIN_SUCCESS "230"
#define ID_LOGIN_FAILED "530"
#define ID_UNKNOWN "530"
#define ID_SYST "215"
#define ID_HELP "214"
#define ID_LIST_BEGIN "150"
#define ID_LIST_END "226"
#define ID_PWD "257"
#define ID_MKD_SUCCESS "257"
#define ID_MKD_FAILURE "550"
#define ID_RMD_SUCCESS "250"
#define ID_RMD_FAILURE "550"
#define ID_QUIT "221"

#define MSG_NEW_CLIENT "newClient"
#define KEY_USER "user"
#define KEY_PASS "password"
#define MSG_PASS_REQUIRED "passRequired"
#define MSG_LOGIN_FAILED "loginFailed"
#define MSG_LOGIN_SUCCESS "loginSuccess"
#define MSG_NOT_LOGGED "clientNotLogged"
#define MSG_PWD "currentDirectoryMsg"
#define MSG_LIST_BEGIN "listBegin"
#define MSG_LIST_END "listEnd"
#define MSG_MKD_SUCCESS "mkdSuccess"
#define MSG_MKD_FAILURE "mkdFailed"
#define MSG_RMD_SUCCESS "rmdSuccess"
#define MSG_RMD_FAILED "rmdFailed"
#define MSG_QUIT "quitSuccess"
#define MSG_SYST "systemInfo"
#define MSG_UNKNOWN "unknownCommand"
#define MSG_HELP "commands"

#define DIR_FORMAT "drwxrwxrwx 0 1000 1000 4096 Sep 24 12:34 "

class Config;
class Protocol;
class Client;

/* Creates commands based on the messages received
 * by the client. Uses polymorphic classes */
class Command {
 protected:
    std::string dirname;
    std::string possible_user;
    std::string possible_pass;

 public:
    virtual void send_response(Config &,
                               Protocol *protocol,
                               Server *,
                               Client &) {}
    virtual ~Command() {}
    static std::string format_cmd(const char * ID,
                                  const char * message,
                                  Config & cfg);
    static std::string format_cmd(const char * ID,
                                  std::string &,
                                  const char * message,
                                  Config & cfg);
};

class CommandCreator : public Command{
 public:
    Command * create_command(std::string &);
    static Command * identify_command(std::string &, std::string &);
};

class UserCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    explicit UserCommand(const std::string &arg);
    ~UserCommand() {}
};

class PassCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    explicit PassCommand(const std::string &arg);
    bool valid_credentials(Config &, Client &);
    ~PassCommand() {}
};

class SystCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~SystCommand() {}
};

class ListCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~ListCommand() override {}
};

class HelpCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~HelpCommand() {}
};

class PwdCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~PwdCommand() {}
};

class MkdCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    explicit MkdCommand(const std::string &arg);
    ~MkdCommand() {}
};

class RmdCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    explicit RmdCommand(const std::string &arg);
    ~RmdCommand() {}
};

class QuitCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~QuitCommand() {}
};

class InvalidCommand : public Command{
 public:
    void send_response(Config &cfg,
                       Protocol *protocol,
                       Server *server,
                       Client &cli) override;
    ~InvalidCommand() {}
};

#endif  // SERVER_COMMAND_H
