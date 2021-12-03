// RIOT
#include <log.h>
#include <periph/cpuid.h>
#ifdef MODULE_VFS
    #include <vfs.h>
#endif

// Project
#include <frames.h>
#include <settings.h>
#ifdef MODULE_QTPY
    #include <qtpy.h>
#endif
#include <wsn.h>


uint64_t cpuid = 0;


void wsn_boot(void)
{
    puts("");
    puts("Platform information, for development: ");
    printf("   sizeof(short)        = %d\n", sizeof(short));
    printf("   sizeof(int)          = %d\n", sizeof(int));
    printf("   sizeof(long)         = %d\n", sizeof(long));
    printf("   sizeof(long long)    = %d\n", sizeof(long long));
    printf("   sizeof(time_t)       = %d\n", sizeof(time_t));
    printf("   sizeof(ztimer_now_t) = %d\n", sizeof(ztimer_now_t));
    printf("   THREAD_EXTRA_STACKSIZE_PRINTF = %d\n", THREAD_EXTRA_STACKSIZE_PRINTF);
    printf("   THREAD_STACKSIZE_DEFAULT      = %d\n", THREAD_STACKSIZE_DEFAULT);
    printf("   THREAD_STACKSIZE_IDLE         = %d\n", THREAD_STACKSIZE_IDLE);
#ifdef ISR_STACKSIZE
    printf("   ISR_STACKSIZE                 = %u\n", ISR_STACKSIZE);
#endif
    puts("");

    /*
     * ID
     *
     * XXX In the cc2539 the CPU id is based on the MAC address, which is
     * defined by two 32bit little-endian words stored in memory: first the
     * most-significat word (IEEE_ADDR_MSWORD), then the less-significat word
     * (IEEE_ADDR_MSWORD). But the CPU id is read from memory as-is.
     *
     * For example if in memory we have 00:4b:12:00 2e:15:40:19 then:
     * - MAC address is 00:12:4B:00:19:40:15:2E
     * - But CPU id is  00:4b:12:00:2e:15:40:19
     *
     */
    uint8_t buffer[CPUID_LEN];
    cpuid_get(buffer);

    for (unsigned int i = 0; i < CPUID_LEN; i++) {
        cpuid = cpuid << 8;
        cpuid |= buffer[i];
    }

    // Storage
    if (IS_USED(MODULE_LITTLEFS2)) {
        if (wsn_mount() == 0) {
            if (IS_USED(MODULE_SETTINGS)) {
                settings_load();
            }

            if (IS_USED(MODULE_FRAMES)) {
                frames_init();
            }
        }
    } else {
        LOG_WARNING("Missing VFS module");
    }

    // Sensors
    #ifdef MODULE_QTPY
        qtpy_init_auto();
    #endif

    // Network
    if (IS_USED(MODULE_GNRC_NETIF)) {
        wsn_network_init();
    }
}
