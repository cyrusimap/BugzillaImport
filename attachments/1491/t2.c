#include <stdio.h>
#include <string.h>
#define IMAP_QUOTAROOT_NONEXISTENT               (2390157849L)

int return_an_error(const char *s)
{
    if (!strcmp(s, "foo"))
	return IMAP_QUOTAROOT_NONEXISTENT;
    return 0;
}

void check_an_error(const char *s)
{
    int r;

    printf("check_an_error(%s)\n", s);
    r = return_an_error(s);
    if (!r)
	printf("OK\n");
    else if (r == IMAP_QUOTAROOT_NONEXISTENT)
	printf("IMAP_QUOTAROOT_NONEXISTENT\n");
    else
	printf("unknown errors %ld\n", (long)r);
}

int main(int argc, char **argv)
{
    check_an_error("foo");
    check_an_error("bar");
    return 0;
}
