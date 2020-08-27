#include <cstdio>
#include <cstdint>

struct seam_string
{
	std::size_t size;
	char data[];
};

extern "C" __declspec(dllexport) void println(seam_string* string)
{
	fwrite(string->data, sizeof(char), string->size, stdout);
}

