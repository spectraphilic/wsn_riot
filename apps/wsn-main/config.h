#ifndef WSN_CONFIG_H
#define WSN_CONFIG_H

#define LOOP_MINUTES 1 // Loop every 1 minute
#define SEND_LOOPS 5 // Send frames every 5 minutes

#ifndef NODE_ID
    #define NODE_ID ""
#endif

#define THREAD_PRIORITY_SENSORS THREAD_PRIORITY_MAIN -3
#define THREAD_PRIORITY_RECV    THREAD_PRIORITY_MAIN -2

#endif
