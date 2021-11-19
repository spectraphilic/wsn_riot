#ifdef MODULE_VFS

// Project
#include <triage.h>
#include <wsn.h>

int cmd_format(int argc, char **argv)
{
    // Arguments
    if (check_argc(argc, 1) < 0) {
        return -1;
    }
    (void)argv;

    // Format
    if (wsn_format() != 0) {
        return -1;
    }

    return 0;
}

#endif
