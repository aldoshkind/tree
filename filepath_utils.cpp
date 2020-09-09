#include "filepath_utils.h"

#include <string.h>
#include <stdlib.h>

int extract_next_level_name(std::string path, std::string &name, std::string &new_path)
{
	size_t name_end_pos = path.find_first_of('/');
	if(name_end_pos == std::string::npos)
	{
		name = path;
		return 0;
	}
	name = path.substr(0, name_end_pos);
	new_path = path.substr(name_end_pos + 1);
	return 0;
}

int extract_last_level_name(const std::string &path, std::string &start, std::string &last)
{
	size_t name_end_pos = path.find_last_of('/');
	if(name_end_pos == std::string::npos)
	{
		// here is a question how to do it correctly. take a look at git history of next two strings
		start = "";
		last = path;
		return 0;
	}
	start = path.substr(0, name_end_pos);
	last = path.substr(name_end_pos + 1);
	return 0;
}

int clean_path(char *path)
{
	if(path == NULL || path[0] != '/')
	{
		return -1;
	}

	char *p = path;

	int start = 1;
	int target = 1;
	int i = 0;
	for(i = 1 ; ; i += 1, target += 1)
	{
		p[target] = path[i];
		if((path[i] == '/') || (path[i] == 0))
		{
			int len = target - start;
			if((target - start) == 0)
			{
				start = target;
				p[start] = path[i];
				target -= 1;
			}
			else if(len == 1 && strncmp(path + start, ".", 1) == 0)
			{
				start = target - 1;
				p[start] = path[i];
				target -= 2;
			}
			else if(len == 2 && strncmp(path + start, "..", 2) == 0)
			{
				int slash_count = 0;
				int j = 0;
				for(j = target - 1 ; j > 1 ; j -= 1)
				{
					if(p[j] == '/')
					{
						slash_count += 1;
					}
					if(slash_count == 2)
					{
						j += 1;
						break;
					}
				}
				int new_target = j;
				target = new_target - 1;
				start = target + 1;
				p[start] = path[i];
			}
			else
			{
				start = target + 1;
			}
		}
		if(path[i] == 0)
		{
			break;
		}
	}

	if(target > 1 && p[target] == '/')
	{
		p[target] = 0;
	}

	return 0;
}

int clean_path(std::string &path)
{
	int res = clean_path((char *)path.c_str());
	path.resize(strlen(path.c_str()));
	return res;
}
