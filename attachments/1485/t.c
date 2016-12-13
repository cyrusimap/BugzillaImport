#include <stdio.h>
#include <string.h>
#define IMAP_INTERNAL                            (-1904809472L)

int return_an_error(const char *s)
{
    if (!strcmp(s, "foo"))
	return IMAP_INTERNAL;
    return 0;
}

void check_an_error(const char *s)
{
    int r;

    printf("check_an_error(%s)\n", s);
    r = return_an_error(s);
    if (!r)
	printf("OK\n");
    else if (r == IMAP_INTERNAL)
	printf("IMAP_INTERNAL\n");
    else
	printf("unknown errors %ld\n", (long)r);
}

int main(int argc, char **argv)
{
    check_an_error("foo");
    check_an_error("bar");
    return 0;
}
