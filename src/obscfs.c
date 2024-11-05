#include <obscfs.h>

char *obsc_readTextFile(const char *_path)
{
	FILE *file;
	char *data = (char *)0;
	size_t data_size = 0;

	file = fopen(_path, "r");

	if (!file)
		return data;

	fseek(file, 0, SEEK_END);
	data_size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	data = (char *)malloc(data_size + 1);
	memset(data, 0, sizeof(*data) * (data_size + 1));

	if (!data)
		return data;

	fread(data, sizeof(*data), data_size, file);
	fclose(file);

	return data;
}