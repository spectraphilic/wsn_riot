#include <errno.h>
#include <log.h>


int check_argc(int argc, int expected)
{
    if (argc != expected) {
        printf("Expected %d arguments, got %d\n", expected, argc);
        return -1;
    }

    return 0;
}
