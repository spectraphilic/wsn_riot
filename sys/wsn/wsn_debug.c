#include <pm_layered.h>


void wsn_debug_pm(void)
{
    printf("PM BLOCKERS: ");
    pm_blocker_t pm_blocker = pm_get_blocker();
    for (unsigned i = 0; i < PM_NUM_MODES; i++) {
        printf("%d ", pm_blocker.blockers[i]);
    }
    printf("\n");
}
