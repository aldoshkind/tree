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

int extract_last_level_name(std::string path, std::string &start, std::string &last)
{
	size_t name_end_pos = path.find_last_of('/');
	if(name_end_pos == std::string::npos)
	{
		start = path;
		return 0;
	}
	start = path.substr(0, name_end_pos);
	last = path.substr(name_end_pos + 1);
	return 0;
}

int clean_path(char *path)
{
	int path_len = strlen(path);

	if(path_len < 1 || path[0] != '/')
	{
		return -1;
	}

	char *p = path;

	int start = 1;
	int target = 1;
	int i = 0;
	for(i = 1 ; i < path_len ; i += 1, target += 1)
	{
		p[target] = path[i];
		if(path[i] == '/')
		{
			if((start - i) == 0)
			{
				p[target] = 0;
				target -= 1;
			}
			if(strncmp(path + start, "./", 2) == 0)
			{
				p[target - 1] = 0;
				target -= 2;
			}

			if(strncmp(path + start, "../", 3) == 0)
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
				memset(p + new_target, 0, target - new_target);
				p[new_target] = 0;
				target = new_target - 1;
			}

			start = i + 1;
			continue;
		}
	}
	p[target] = 0;

	return 0;
}
