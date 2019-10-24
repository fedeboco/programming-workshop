#include <string>

#include "Command.h"
#include "Config.h"
#include "Protocol.h"

/* Sends response to USER command */
void UserCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    std::string CMD = format_cmd(ID_PASS_REQUIRED, MSG_PASS_REQUIRED, cfg);
    protocol->send_string(CMD, cli.socket_id);
    cli.possible_user = possible_user;
}

/* Sends response to PASS command attempting to
 * log in if credentials match the config file */
void PassCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    if (valid_credentials(cfg, cli)) {
        std::string CMD = format_cmd(ID_LOGIN_SUCCESS,
                                     MSG_LOGIN_SUCCESS,
                                     cfg);
        protocol->send_string(CMD, cli.socket_id);
        cli.logged = true;
    } else {
        std::string CMD = format_cmd(ID_LOGIN_FAILED,
                                     MSG_LOGIN_FAILED,
                                     cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Checks if credentials match with the config file */
bool PassCommand::valid_credentials(Config & cfg, Client & cli) {
    return (cli.possible_user == cfg.get_message(KEY_USER)) &&
           (possible_pass == cfg.get_message(KEY_PASS));
}

/* Sends server's system info if logged*/
void SystCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    if (cli.logged) {
        std::string CMD = format_cmd(ID_SYST, MSG_SYST, cfg);
        protocol->send_string(CMD, cli.socket_id);
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Sends list of directories if logged */
void ListCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    if (cli.logged) {
        std::string CMD = format_cmd(ID_LIST_BEGIN, MSG_LIST_BEGIN, cfg);
        protocol->send_string(CMD, cli.socket_id);
        if (server->directories_size() > 0)
            protocol->send_string(server->get_list(), cli.socket_id);
        CMD = format_cmd(ID_LIST_END, MSG_LIST_END, cfg);
        protocol->send_string(CMD, cli.socket_id);
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Sends valid commands if logged */
void HelpCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    if (cli.logged) {
        std::string CMD = format_cmd(ID_HELP, MSG_HELP, cfg);
        protocol->send_string(CMD, cli.socket_id);
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Sends current directory if logged */
void PwdCommand::send_response(Config &cfg,
                               Protocol *protocol,
                               Server *server,
                               Client &cli) {
    if (cli.logged) {
        std::string CMD = format_cmd(ID_PWD, MSG_PWD, cfg);
        protocol->send_string(CMD, cli.socket_id);
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Creates a directory if it's not present and
 * sends a response to the client if the operation
 * has been successful or not */
void MkdCommand::send_response(Config &cfg,
                               Protocol *protocol,
                               Server *server,
                               Client &cli) {
    if (cli.logged) {
        if (server->add_directory(dirname)) {
            std::string CMD = format_cmd(ID_MKD_SUCCESS,
                                         dirname,
                                         MSG_MKD_SUCCESS,
                                         cfg);
            protocol->send_string(CMD, cli.socket_id);
        } else {
            std::string CMD = format_cmd(ID_MKD_FAILURE,
                                         MSG_MKD_FAILURE,
                                         cfg);
            protocol->send_string(CMD, cli.socket_id);
        }
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Removes a directory if it's present and
 * sends a response to the client if the operation
 * has been successful or not */
void RmdCommand::send_response(Config &cfg,
                               Protocol *protocol,
                               Server *server,
                               Client &cli) {
    if (cli.logged) {
        if (server->remove_directory(dirname)) {
            std::string CMD = format_cmd(ID_RMD_SUCCESS,
                                         dirname,
                                         MSG_RMD_SUCCESS,
                                         cfg);
            protocol->send_string(CMD, cli.socket_id);
        } else {
            std::string CMD = format_cmd(ID_RMD_FAILURE,
                                         MSG_RMD_FAILED,
                                         cfg);
            protocol->send_string(CMD, cli.socket_id);
        }
    } else {
        std::string CMD = format_cmd(ID_NOT_LOGGED, MSG_NOT_LOGGED, cfg);
        protocol->send_string(CMD, cli.socket_id);
    }
}

/* Sets quit signal in the client and tells the client
 * to disconnect */
void QuitCommand::send_response(Config &cfg,
                                Protocol *protocol,
                                Server *server,
                                Client &cli) {
    cli.quit = true;
    std::string CMD = format_cmd(ID_QUIT, MSG_QUIT, cfg);
    protocol->send_string(CMD, cli.socket_id);
}

/* If an unknown command has been received, warns the user */
void InvalidCommand::send_response(Config &cfg,
                                   Protocol *protocol,
                                   Server *server,
                                   Client &cli) {
    std::string CMD = format_cmd(ID_UNKNOWN, MSG_UNKNOWN, cfg);
    protocol->send_string(CMD, cli.socket_id);
}

/* Creates a command with some validation */
Command * CommandCreator::create_command(std::string & input) {
    std::string cmd_argument;
    size_t argument_start_pos;

    if (input.length() > 5) {
        argument_start_pos = input.find(' ') + 1;
        cmd_argument = input.substr(argument_start_pos, std::string::npos);
        input = input.substr(0, argument_start_pos - 1);
    } else {
        input.pop_back();  // removes new line char
    }

    Command * cmd = identify_command(input, cmd_argument);
    return cmd;
}

/* Checks if the command matches with the commands specified
 * in the config file */
Command * CommandCreator::identify_command(std::string & cmd,
                                           std::string & arg) {
    std::string cmds_id[] = {CMD_USER, CMD_PASS, CMD_SYST,
                             CMD_LIST, CMD_HELP, CMD_PWD,
                             CMD_MKD, CMD_RMD, CMD_QUIT};

    if (!cmd.compare(cmds_id[0]))
        return new UserCommand(arg);
    if (!cmd.compare(cmds_id[1]))
        return new PassCommand(arg);
    if (!cmd.compare(cmds_id[2]))
        return new SystCommand;
    if (!cmd.compare(cmds_id[3]))
        return new ListCommand;
    if (!cmd.compare(cmds_id[4]))
        return new HelpCommand;
    if (!cmd.compare(cmds_id[5]))
        return new PwdCommand;
    if (!cmd.compare(cmds_id[6]))
        return new MkdCommand(arg);
    if (!cmd.compare(cmds_id[7]))
        return new RmdCommand(arg);
    if (!cmd.compare(cmds_id[8]))
        return new QuitCommand;
    return new InvalidCommand;
}

/* Formats response to send */
std::string Command::format_cmd(const char * ID,
                                const char * message,
                                Config & cfg) {
    return std::string(ID) + " " + cfg.get_message(message);
}

/* Formats response to send with directory name */
std::string Command::format_cmd(const char * ID,
                                std::string & dir,
                                const char * message,
                                Config & cfg) {
    dir.pop_back();  // removes new line char
    return std::string(ID) +
           " " +
           "\"" + dir + "\" " +
           cfg.get_message(message);
}

/* Constructor: creates user command with a possible user */
UserCommand::UserCommand(const std::string & arg) { possible_user = arg;}

/* Constructor: creates pass command with a possible password */
PassCommand::PassCommand(const std::string & arg) { possible_pass = arg;}

/* Constructor: creates mkd command with a directory name */
MkdCommand::MkdCommand(const std::string &arg) { dirname = arg;}

/* Constructor: creates rmd command with a directory name */
RmdCommand::RmdCommand(const std::string &arg) { dirname = arg;}

