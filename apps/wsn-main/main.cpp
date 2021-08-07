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
#include <saul_reg.h>
#include <timex.h>
#include <ztimer.h>

// Project
#include "triage.h"
#include "wsn.h"


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

#define SLEEP 5 // seconds
#define RCV_QUEUE_SIZE 8

#ifndef BASETIME
    #define BASETIME 0
#endif

static unsigned int base_time = BASETIME;


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
            n = sscanf((char*)pkt->data, "pong %u", &base_time);
            if (n == 1) {
                ztimer_now_t now = ztimer_now(ZTIMER);
                base_time -= now / TICKS_PER_SEC;
                printf("TIME %u\n", base_time);
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

static void *_eventloop(void *arg)
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

kernel_pid_t gnrc_pktdump_pid = KERNEL_PID_UNDEF;
static char _stack[THREAD_STACKSIZE_MAIN];
static void connect_loop(void)
{
    gnrc_pktdump_pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN -1,
                         THREAD_CREATE_STACKTEST,
                         _eventloop, NULL, "pktdump");
    gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(
        GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);

    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    // Send ping messages to the gateway until it replies
    const char msg[] = "ping";
    const uint8_t size = strlen(msg);
    while (base_time == 0) {
        send((uint8_t*)msg, size);
        LOG_INFO("%s\n", msg);
        ztimer_sleep(ZTIMER, SLEEP * TICKS_PER_SEC);
    }
}


int main(void)
{
    uint8_t buffer[128];
    nanocbor_encoder_t enc;
    phydat_t res;

    LED0_ON;
    //test_utils_interactive_sync();

    // Boot
    wsn_boot();
    LOG_INFO("app=wsn-main board=%s mcu=%s\n", RIOT_BOARD, RIOT_MCU);
    LOG_INFO("This program loops forever, sleeping for %ds in every loop.\n", SLEEP);
    LOG_INFO("basetime=%d\n", base_time);

    // Connect to gateway
    connect_loop();

    // Main loop
    for (unsigned int loop=0; ; loop++) {
        LOG_INFO("Loop=%u\n", loop);

        // Read sensors and fill buffer
        nanocbor_encoder_init(&enc, buffer, sizeof(buffer));
        nanocbor_fmt_array_indefinite(&enc);

        // Timestamp
        ztimer_now_t now = ztimer_now(ZTIMER);
        nanocbor_fmt_uint(&enc, 0);
        nanocbor_fmt_uint(&enc, base_time + now / TICKS_PER_SEC);

        // Serial number
        nanocbor_fmt_uint(&enc, 1);
        nanocbor_fmt_uint(&enc, cpuid);

        // Name (Node Identifier)
        nanocbor_fmt_uint(&enc, 2);
        nanocbor_put_tstr(&enc, NODE_ID);

        // Frame sequence number
        nanocbor_fmt_uint(&enc, 3);
        nanocbor_fmt_uint(&enc, loop);

        saul_reg_t *dev = saul_reg;
        while (dev) {
            uint8_t type = dev->driver->type;
            if ((type & SAUL_CAT_MASK) == SAUL_CAT_SENSE) {
                printf("- %-15s %3d %-15s ", dev->name, type, saul_class_to_str(type));

                uint8_t id = type & SAUL_ID_MASK;
                if (id == SAUL_SENSE_ID_TEMP || id == SAUL_SENSE_ID_HUM || id == SAUL_SENSE_ID_PRESS) {
                    // Read and add to CBOR
                    int dim = saul_reg_read(dev, &res);
                    if (dim <= 0) {
                        printf("ERROR\n");
                    } else {
                        nanocbor_fmt_uint(&enc, type);
                        for (int j=0; j < dim; j++) {
                            int value = res.val[j];
                            printf("%6d ", value);
                            nanocbor_fmt_int(&enc, value);
                        }
                        printf("unit=%-2s scale=%d\n", phydat_unit_to_str(res.unit), res.scale);
                    }
                } else {
                    printf("Not in the whitelist\n");
                }
            }

            // Next
            dev = dev->next;
        }

        nanocbor_fmt_end_indefinite(&enc);
        size_t required = nanocbor_encoded_len(&enc);
        assert(required);

        printf("CBOR = ");
        for (size_t k=0; k < required; k++) {
            printf("%02x", buffer[k]);
        }
        printf("\n");

        // Send
        send(buffer, required);

        // Done
        LOG_INFO("Loop=%u DONE\n", loop);
        LED0_OFF;
        ztimer_sleep(ZTIMER, SLEEP * TICKS_PER_SEC);
        LED0_ON;
    }

    return 0;
}
