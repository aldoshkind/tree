#pragma once

#include <string>

namespace tree::filepath_utils
{

int clean_path(char *path);
int clean_path(std::string &path);
int extract_next_level_name(std::string path, std::string &name, std::string &new_path);
int extract_last_level_name(const std::string &path, std::string &path_start, std::string &last);

}
