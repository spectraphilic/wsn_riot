#ifndef WSN_CONFIG_H
#define WSN_CONFIG_H

#define LOOP_SECONDS 30 // Loop every 30 seconds
#define SEND_SECONDS 60 // Send frames every 60 seconds

#ifndef NODE_ID
    #define NODE_ID ""
#endif

#define THREAD_PRIORITY_SENSORS THREAD_PRIORITY_MAIN -2
#define THREAD_PRIORITY_RECV    THREAD_PRIORITY_MAIN -3

#endif
