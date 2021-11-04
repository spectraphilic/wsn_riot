// Riot
#include <log.h>
#include <msg.h>
#include <net/gnrc.h>
#include <od.h>
#include <shell.h>
#include <thread.h>

// Project
#include <wsn.h>
#include "common.h"
#include "config.h"


#define QUEUE_SIZE 8

static kernel_pid_t pid = KERNEL_PID_UNDEF;
static char stack[THREAD_STACKSIZE_MAIN];

static void dump_snip(gnrc_pktsnip_t *pkt)
{
    size_t hdr_len = 0;

    switch (pkt->type) {
        case GNRC_NETTYPE_NETIF:
            printf("NETTYPE_NETIF (%i)\n", pkt->type);
            if (IS_USED(MODULE_GNRC_NETIF_HDR)) {
                gnrc_netif_hdr_print((gnrc_netif_hdr_t*) pkt->data);
                hdr_len = pkt->size;
            }
            break;
        case GNRC_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)\n", pkt->type);
            printf("ECHO:  %.*s\n", pkt->size, (char*)pkt->data);
            // Command
//          shell_run_once(shell_commands, (char*)pkt->data, pkt->size);

            unsigned int time;
            int n = sscanf((char*)pkt->data, "time %u", &time);
            if (n == 1) {
                wsn_time_set(time);
            }
            break;
        default:
            printf("NETTYPE_UNKNOWN (%i)\n", pkt->type);
            break;
    }
    if (hdr_len < pkt->size) {
        size_t size = pkt->size - hdr_len;

        od_hex_dump(((uint8_t *)pkt->data) + hdr_len, size, OD_WIDTH_DEFAULT);
    }
}

static void dump_pkt(gnrc_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    gnrc_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3u byte, type: ", snips,
               (unsigned int)snip->size);
        dump_snip(snip);
        ++snips;
        size += snip->size;
        snip = snip->next;
    }

    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    gnrc_pktbuf_release(pkt);
}

static void *task_func(void *arg)
{
    (void)arg;

    // Setup the message queue
    msg_t msg_queue[QUEUE_SIZE];
    msg_init_queue(msg_queue, QUEUE_SIZE);

    // Prepare message reply
    msg_t reply;
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    // Forever wait for new messages and handle
    msg_t msg;
    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:");
                dump_pkt((gnrc_pktsnip_t*) msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                dump_pkt((gnrc_pktsnip_t*) msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                puts("PKTDUMP: received something unexpected");
                break;
        }
    }

    // Never reached
    return NULL;
}

void thread_recv_start(void)
{
    if (pid == KERNEL_PID_UNDEF) {
        // Create the thread
        pid = thread_create(
            stack,
            sizeof(stack),
            THREAD_PRIORITY_RECV,
            THREAD_CREATE_STACKTEST,
            task_func,
            NULL,
            "network-tap"
        );

        // Register the thread to receive events from the network stack
        gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL, pid);
        int error = gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
        if (error) {
            LOG_ERROR("gnrc_netreg_register failed");
        }
    }
}
