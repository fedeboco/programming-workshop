#include <string>
#include "Config.h"

/* Constructor: saves file and configuration specified in file*/
Config::Config(char * config_file) {
    std::string aux_str1;
    std::string aux_str2;
    configstream.open(config_file, std::ios::in);
    while (!configstream.eof()) {
        std::getline(configstream, aux_str1, '=');
        std::getline(configstream, aux_str2, '\n');
        aux_str2.push_back('\n');
        configmap.emplace(aux_str1, aux_str2);
    }
    configstream.close();
}

/* Gets message loaded in memory from specified config file.*/
std::string Config::get_message(const std::string & key) {
    mapit it = configmap.find(key);
    return it->second;
}
