#ifdef MODULE_GNRC

// Riot
#include <log.h>
#include <nanocbor/nanocbor.h>
#include <net/gnrc/pktdump.h>
#include <net/gnrc.h>
#include <od.h>

// Project
#include <frames.h>
#include "common.h"
#include "config.h"


static int handle_cmd(const char *data, size_t len)
{
    // Command
    // TODO Use the shell, for this we need to call handle_input_line from
    // RIOT/sys/shell/shell.c but it's declared as static. This has been
    // asked before, see https://github.com/RIOT-OS/RIOT/issues/4967
    // TODO Send a PR
    // handle_input_line(shell_commands, (char*)pkt->data));

    int n;

    (void)len; // FIXME

    // time
    ztimer_now_t time;
    n = sscanf(data, "time %ld", &time);
    if (n == 1) {
        wsn_time_set(time);
        return -1;
    }

    // ack
    if (strcmp(data, "ack") == 0) {
        n = frames_drop();
        if (n > 0) {
            send_frame();
        }
    }

    return 0;
}


static int handle_pkt(gnrc_pktsnip_t *pkt)
{
    LOG_INFO("Recv \"%.*s\"\n", pkt->size, (char*)pkt->data);

#if IS_USED(MODULE_SX127X)
    // Parse CBOR
    nanocbor_value_t decoder;
    nanocbor_decoder_init(&decoder, pkt->data, pkt->size);

    nanocbor_value_t array;
    int rc = nanocbor_enter_array(&decoder, &array);
    if (rc < 0) {
        LOG_ERROR("Expected CBOR array");
        return -1;
    }

    const uint8_t *buf = NULL;
    size_t buf_len;

    uint8_t source_addr, target_addr;
    rc = nanocbor_get_uint8(&array, &source_addr);
    rc = nanocbor_get_uint8(&array, &target_addr);

    uint8_t address;
    wsn_network_get_opt(NULL, NETOPT_ADDRESS, &address, sizeof(address));
    LOG_INFO("Packet addressed to %u (my address is %u)", target_addr, address);
    if (target_addr != address && target_addr != 0) {
        return 0;
    }

    while (!nanocbor_at_end(&array)) {
        rc = nanocbor_get_tstr(&array, &buf, &buf_len);
        handle_cmd((const char*)buf, buf_len);
    }
#else
    // TODO Handle CBOR array as well, but without the source/target address.
    // Problem is wsn_pi/wsn_xbee.py needs to send CBOR, but waspmote expects
    // str.
    handle_cmd(pkt->data, pkt->size);
#endif


    return 0;
}


/**
 * @brief   PID of the pktdump thread
 */
kernel_pid_t gnrc_pktdump_pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktdump thread
 */
static char _stack[GNRC_PKTDUMP_STACKSIZE];

static void _dump_snip(gnrc_pktsnip_t *pkt)
{
    size_t hdr_len = 0;

    switch (pkt->type) {
        case GNRC_NETTYPE_NETIF:
            printf("NETTYPE_NETIF (%i)\n", pkt->type);
            if (IS_USED(MODULE_GNRC_NETIF_HDR)) {
                gnrc_netif_hdr_print(pkt->data);
                hdr_len = pkt->size;
            }
            break;
        case GNRC_NETTYPE_UNDEF:
            printf("NETTYPE_UNDEF (%i)\n", pkt->type);
            handle_pkt(pkt);
            break;
#if IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN)
        case GNRC_NETTYPE_SIXLOWPAN:
            printf("NETTYPE_SIXLOWPAN (%i)\n", pkt->type);
            if (IS_USED(MODULE_SIXLOWPAN)) {
                sixlowpan_print(pkt->data, pkt->size);
                hdr_len = pkt->size;
            }
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_SIXLOWPAN) */
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6)
        case GNRC_NETTYPE_IPV6:
            printf("NETTYPE_IPV6 (%i)\n", pkt->type);
            if (IS_USED(MODULE_IPV6_HDR)) {
                ipv6_hdr_print(pkt->data);
                hdr_len = sizeof(ipv6_hdr_t);
            }
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_IPV6) */
#if IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT)
        case GNRC_NETTYPE_IPV6_EXT:
            printf("NETTYPE_IPV6_EXT (%i)\n", pkt->type);
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_IPV6_EXT) */
#if IS_USED(MODULE_GNRC_NETTYPE_ICMPV6)
        case GNRC_NETTYPE_ICMPV6:
            printf("NETTYPE_ICMPV6 (%i)\n", pkt->type);
            if (IS_USED(MODULE_ICMPV6)) {
                icmpv6_hdr_print(pkt->data);
                hdr_len = sizeof(icmpv6_hdr_t);
            }
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_ICMPV6) */
#if IS_USED(MODULE_GNRC_NETTYPE_TCP)
        case GNRC_NETTYPE_TCP:
            printf("NETTYPE_TCP (%i)\n", pkt->type);
            if (IS_USED(MODULE_TCP)) {
                tcp_hdr_print(pkt->data);
                hdr_len = sizeof(tcp_hdr_t);
            }
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_TCP) */
#if IS_USED(MODULE_GNRC_NETTYPE_UDP)
        case GNRC_NETTYPE_UDP:
            printf("NETTYPE_UDP (%i)\n", pkt->type);
            if (IS_USED(MODULE_UDP)) {
                udp_hdr_print(pkt->data);
                hdr_len = sizeof(udp_hdr_t);
            }
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_UDP) */
#if IS_USED(MODULE_GNRC_NETTYPE_CCN)
        case GNRC_NETTYPE_CCN_CHUNK:
            printf("GNRC_NETTYPE_CCN_CHUNK (%i)\n", pkt->type);
            printf("Content is: %.*s\n", (int)pkt->size, (char*)pkt->data);
            hdr_len = pkt->size;
            break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_CCN) */
#if IS_USED(MODULE_GNRC_NETTYPE_NDN)
    case GNRC_NETTYPE_NDN:
            printf("NETTYPE_NDN (%i)\n", pkt->type);
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_NDN) */
#if IS_USED(MODULE_GNRC_NETTYPE_LORAWAN)
    case GNRC_NETTYPE_LORAWAN:
            printf("NETTYPE_LORAWAN (%i)\n", pkt->type);
        break;
#endif  /* IS_USED(MODULE_GNRC_NETTYPE_LORAWAN) */
#ifdef TEST_SUITES
        case GNRC_NETTYPE_TEST:
            printf("NETTYPE_TEST (%i)\n", pkt->type);
            break;
#endif
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
    msg_t msg_queue[GNRC_PKTDUMP_MSG_QUEUE_SIZE];

    /* setup the message queue */
    msg_init_queue(msg_queue, GNRC_PKTDUMP_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    LOG_INFO("Network listening thread started");
    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                puts("PKTDUMP: data received:");
                _dump(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                puts("PKTDUMP: data to send:");
                _dump(msg.content.ptr);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                LOG_WARNING("Unexpected network message");
                break;
        }
    }

    /* never reached */
    return NULL;
}

kernel_pid_t thread_recv_start(void)
{
    if (gnrc_pktdump_pid == KERNEL_PID_UNDEF) {
        gnrc_pktdump_pid = thread_create(
            _stack,
            sizeof(_stack),
            THREAD_PRIORITY_RECV,
            THREAD_CREATE_STACKTEST,
            _eventloop,
            NULL,
            "network-tap");
    }
    return gnrc_pktdump_pid;
}

#endif
