#include <assert.h>

// RIOT
#include <board.h>
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <net/netif.h>
#include <net/gnrc/netif.h>
#include <net/gnrc/netif/hdr.h>
#include <net/gnrc/netreg.h>
#include <od.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include <sensors.h>
#include <triage.h>
#include <wsn.h>
#include "config.h"


#ifdef MODULE_ZTIMER_MSEC
    #define ZTIMER ZTIMER_MSEC
    #define TICKS_PER_SEC MS_PER_SEC
#else
    #define ZTIMER ZTIMER_USEC
    #define TICKS_PER_SEC US_PER_SEC
#endif

#ifndef NODE_ID
    #define NODE_ID ""
#endif


static int send(const uint8_t *data, size_t size)
{
    gnrc_pktsnip_t *pkt, *hdr;
    gnrc_netif_hdr_t *nethdr;

    netif_t *netif = netif_get_by_id(2);

    // Broadcast
    uint8_t addr[GNRC_NETIF_L2ADDR_MAXLEN];
    size_t addr_len = 0;
    uint8_t flags = 0x00;
    if (netif) {
        flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;
    }

    // Data
    printf("gnrc_pktbuf_add(...) size = %d\n", size);
    pkt = gnrc_pktbuf_add(NULL, data, size, GNRC_NETTYPE_UNDEF);
    if (pkt == NULL) {
        LOG_ERROR("unable to copy data to packet buffer\n");
        return -1;
    }

    // Header
    hdr = gnrc_netif_hdr_build(NULL, 0, addr, addr_len);
    if (hdr == NULL) {
        LOG_ERROR("packet buffer full\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }
    LL_PREPEND(pkt, hdr);
    nethdr = (gnrc_netif_hdr_t *)hdr->data;
    nethdr->flags = flags;

    // Send
    if (gnrc_netif_send((gnrc_netif_t *)netif, pkt) < 1) {
        LOG_ERROR("unable to send");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    return 0;
}


static void send_frames()
{
    uint8_t buffer[150];
    uint8_t len;
    int success = 0;

    while (1) {
        // Load frame
        int n = wsn_load_frame(buffer, &len);
        if (n <= 0)
            break;

        // Send frame
        int error = send(buffer, len);
        if (error < 0)
            break;
        success++;

        // Drop frame
        n = wsn_drop_frame();
        if (n <= 0)
            break;
    }
    LOG_INFO("%d frames sent\n", success);
}


static void _dump_snip(gnrc_pktsnip_t *pkt)
{
    size_t hdr_len = 0;
    int n;

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
            // pong
            unsigned int time;
            n = sscanf((char*)pkt->data, "pong %u", &time);
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

static void _dump(gnrc_pktsnip_t *pkt)
{
    int snips = 0;
    int size = 0;
    gnrc_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        printf("~~ SNIP %2i - size: %3u byte, type: ", snips,
               (unsigned int)snip->size);
        _dump_snip(snip);
        ++snips;
        size += snip->size;
        snip = snip->next;
    }

    printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
    gnrc_pktbuf_release(pkt);
}

kernel_pid_t eventloop_pid = KERNEL_PID_UNDEF;
static char eventloop_stack[THREAD_STACKSIZE_MAIN];
static void *eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;
    msg_t msg_queue[RCV_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, RCV_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:");
                _dump((gnrc_pktsnip_t*) msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                _dump((gnrc_pktsnip_t*) msg.content.ptr);
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

    /* never reached */
    return NULL;
}

static void start_eventloop(void)
{
    // Create (and start) eventloop thread
    eventloop_pid = thread_create(
        eventloop_stack,
        sizeof(eventloop_stack),
        THREAD_PRIORITY_MAIN -1,
        THREAD_CREATE_STACKTEST,
        eventloop,
        NULL,
        "pktdump"
    );

    // Register the eventloop to receive events from the network stack
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(
        GNRC_NETREG_DEMUX_CTX_ALL, eventloop_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    // Send ping messages to the gateway until it replies
    const char msg[] = "ping";
    const uint8_t size = strlen(msg);
    while (wsn_time_basetime() == 0) {
        send((uint8_t*)msg, size);
        LOG_INFO("%s\n", msg);
        ztimer_sleep(ZTIMER, 10 * TICKS_PER_SEC); // 20s
    }
}

int main(void)
{
    uint8_t buffer[150];
    nanocbor_encoder_t enc;
    phyval_t res;

    LED0_ON;
    //test_utils_interactive_sync();

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for %ds in every loop.\n", LOOP_SECONDS);
    LOG_INFO("basetime=%d\n", wsn_time_basetime());

    // Connect to gateway
    start_eventloop();

    // Main loop
    for (unsigned int loop=0; ; loop++) {
        LOG_INFO("Loop=%u\n", loop);
        time_t time = wsn_time_get();

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

        // Timestamp
        nanocbor_fmt_uint(&enc, 0);
        nanocbor_fmt_uint(&enc, time);

        // Serial number
        nanocbor_fmt_uint(&enc, 1);
        nanocbor_fmt_uint(&enc, cpuid);

        // Name (Node Identifier)
        nanocbor_fmt_uint(&enc, 2);
        nanocbor_put_tstr(&enc, NODE_ID);

        // Frame sequence number
        nanocbor_fmt_uint(&enc, 3);
        nanocbor_fmt_uint(&enc, loop);

        sensor_t *sensor = sensors_list;
        while (sensor) {
            printf("%s:\n", sensor->name);
            while (sensor->read(sensor->dev, &res)) {
                printf("%6ld unit=%-2s scale=%d\n", res.value, phydat_unit_to_str(res.unit), res.scale);
                nanocbor_fmt_int(&enc, res.value);
            }
            sensor = sensor->next;
        }

        nanocbor_fmt_end_indefinite(&enc);
        size_t len = nanocbor_encoded_len(&enc);
        assert(len);

        printf("CBOR = ");
        for (size_t k=0; k < len; k++) {
            printf("%02x", buffer[k]);
        }
        printf("\n");

        // Save the frame
        wsn_save_frame(time, buffer, len);

        // Send
        if ((loop + 1) % LOOPS_SEND == 0) {
            send_frames();
        }

        // Done
        LOG_INFO("Loop=%u DONE\n", loop);
        LED0_OFF;
        ztimer_sleep(ZTIMER, LOOP_SECONDS * TICKS_PER_SEC);
        LED0_ON;
    }

    return 0;
}
