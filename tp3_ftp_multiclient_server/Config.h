#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string>::iterator mapit;

/* Stores configuration file parameters in memory */
class Config {
 private:
    std::ifstream configstream;
    std::unordered_map<std::string, std::string> configmap;

 public:
    explicit Config(char * config_file);
    std::string get_message(const std::string &key);
};

#endif  // SERVER_CONFIG_H
