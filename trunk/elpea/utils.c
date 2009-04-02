#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

gsize get_file_size (const char *path)
{
	struct stat s = {0};
	stat (path, &s);
	return s.st_size;
}


