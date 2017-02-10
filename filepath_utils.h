#pragma once

#include <string>

int clean_resmgr_path(char *path);
int extract_next_level_name(std::string path, std::string &name, std::string &new_path);
