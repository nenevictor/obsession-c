#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PACKER_MAX_ARRAY_LINE_LEN 70
#define PACKER_TAB 2

char *string_to_uppercase(char *s)
{
	char *temp = s;

	while (*temp)
	{
		*temp = toupper((unsigned char)*temp);
		temp++;
	}

	return s;
}

char *load_file(
	const char *_path,
	size_t *_size,
	const char *_mode)
{
	FILE *file;
	char *buffer;
	size_t size;

	file = fopen(_path, _mode);

	if (!file)
	{
		if (_size)
		{
			*_size = 0;
		}
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size = (size_t)ftell(file);

	if (!strcmp("r", _mode))
	{
		buffer = (char *)malloc(size + 1);
		buffer[size] = 0;
	}
	else
	{
		buffer = (char *)malloc(size);
	}

	if (!buffer)
	{
		if (_size)
		{
			*_size = 0;
		}
		return NULL;
	}

	memset(buffer, 0, size);

	fseek(file, 0, SEEK_SET);
	fread(buffer, sizeof(char), size, file);
	fclose(file);

	if (_size)
	{
		*_size = size;
	}

	return buffer;
}

char *load_file_string(
	const char *_path,
	size_t *_size)
{
	return load_file(_path, _size, "r");
}

char *load_file_binary(
	const char *_path,
	size_t *_size)
{
	return load_file(_path, _size, "rb");
}

const char *string_get_file_name(
	const char *_path)
{
	const char *path_delims = "\\/";
	const char *name = NULL;
	const char *path_iter = _path;

	if (!_path)
		return _path;

	while ((path_iter = strpbrk(path_iter, path_delims)))
	{
		name = path_iter++;
	}

	if (!name)
		return _path;

	return name + 1;
}

/* Stole this part from RAYLIB code.*/
const char *string_get_file_ext(const char *_path)
{
	const char *dot = strrchr(_path, '.');

	if (!dot || dot == _path)
		return _path;

	return dot;
}

void string_replace(
	char *_string,
	const size_t _string_size,
	const char _find,
	const char _replace_with)
{
	char *str_iter = _string;
	while ((size_t)(str_iter - _string) < _string_size &&
		   *str_iter != 0)
	{
		if (*str_iter == _find)
			*str_iter = _replace_with;
		str_iter++;
	}
}

int main(int _argc, const char **_argv)
{
	const char *mode, *input_path, *output_path;

	size_t i = 0, j, temp_line_length = 0;

	char *input_data;
	size_t input_data_size;
	FILE *output_file;

	char output_file_prefix[256];
	const size_t hex_code_len = 6;

	if (_argc <= 3)
	{
		printf("[ ERROR ]:\t too few arguments!\n");
		return EXIT_FAILURE;
	}

	mode = _argv[1];
	input_path = _argv[2];
	output_path = _argv[3];

	printf(
		"Input: %s; Output: %s;\n",
		input_path,
		output_path);

	if (!strcmp("r", mode))
	{
		input_data = load_file_string(input_path, &input_data_size);
	}
	else if (!strcmp("rb", mode))
	{
		input_data = load_file_binary(input_path, &input_data_size);
	}
	else
	{
		return 1;
	}

	if (!input_data || !input_data_size)
	{
		printf(
			"[ ERROR ]:\t cannot open input file! %s\n",
			input_path);
		return EXIT_FAILURE;
	}

	output_file = fopen(output_path, "w");
	if (!output_file)
	{
		printf(
			"[ ERROR ]:\t cannot save output file! %s\n",
			output_path);
		return EXIT_FAILURE;
	}

	memset(
		output_file_prefix,
		0,
		sizeof(output_file_prefix));
	strcpy(output_file_prefix, input_path);
	string_to_uppercase(output_file_prefix);
	string_replace(
		output_file_prefix,
		strlen(output_file_prefix),
		'.',
		'_');
	string_replace(
		output_file_prefix,
		strlen(output_file_prefix),
		'-',
		'_');
	string_replace(
		output_file_prefix,
		strlen(output_file_prefix),
		'\\',
		'_');
	string_replace(
		output_file_prefix,
		strlen(output_file_prefix),
		'/',
		'_');

	fprintf(
		output_file,
		"%s",
		"/* Simple packer. 1.1.0 */\n\n");

	fprintf(
		output_file,
		"#define %s_SIZE %u\n"
		"#define %s_EXT \"%s\"\n\n\n"
		"static const unsigned char %s[] = {\n",
		output_file_prefix,
		(unsigned int)input_data_size,
		output_file_prefix,
		string_get_file_ext(input_path),
		output_file_prefix);

	while (i < input_data_size)
	{
		j = 0;
		if (!temp_line_length)
		{
			while (j < PACKER_TAB)
			{
				fputc(' ', output_file);
				j++;
			}
			temp_line_length += PACKER_TAB;
		}

		fprintf(
			output_file,
			"0x%.2x, ",
			(unsigned char)(input_data[i]));
		temp_line_length += hex_code_len;

		if (temp_line_length + hex_code_len >=
			PACKER_MAX_ARRAY_LINE_LEN)
		{
			fputc('\n', output_file);
			temp_line_length = 0;
		}

		i++;
	}

	fputc('\n', output_file);
	j = 0;
	while (j < PACKER_TAB)
	{
		fputc(' ', output_file);
		j++;
	}

	fprintf(
		output_file,
		"0x00");

	fprintf(
		output_file,
		"\n};\n");

	free(input_data);
	fclose(output_file);
	return EXIT_SUCCESS;
}