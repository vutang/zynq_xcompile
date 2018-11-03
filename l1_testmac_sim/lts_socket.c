/*
* @Author: vutang
* @Date:   2018-11-02 13:54:24
* @Last Modified by:   vutang
* @Last Modified time: 2018-11-02 17:27:45
*/

#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/ether.h>
#include <string.h>

#include "logger.h"
#include "lts_socket.h"

#define DEFAULT_IF	"eth0"
#define ETHER_TYPE	0x5a5b

/*Socket file description*/
static int tx_skt_fd = -1;
static struct sockaddr_ll tx_skt_addr;
static char dest_mac[6] = {0xda, 0x02, 0x03, 0x04, 0x05, 0x06};

static struct ifreq tx_if_mac;

int lts_txskt_open() {
	/*IFNAMESIZ is defined in net/if.h*/
	char ifName[IFNAMSIZ];
	struct ifreq tx_if_idx;

	/* Open RAW socket to send on */
	if ((tx_skt_fd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    LOG_ERROR("Create skt fail");
	    return -1;
	}

	/* Get interface name */
	strcpy(ifName, DEFAULT_IF);

	/* Get the index of the interface to send on */
	memset(&tx_if_idx, 0, sizeof(struct ifreq));
	strncpy(tx_if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(tx_skt_fd, SIOCGIFINDEX, &tx_if_idx) < 0) {
	    LOG_ERROR("SIOCGIFINDEX: Get interface index fail");
	    return -1;
	}

	/* Get the MAC address of the interface to send on */
	memset(&tx_if_mac, 0, sizeof(struct ifreq));
	strncpy(tx_if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(tx_skt_fd, SIOCGIFHWADDR, &tx_if_mac) < 0) {
	    LOG_ERROR("SIOCGIFHWADDR: Get interface MAC addr fail");
	    return -1;
	}

	/* Index of the network device */
	tx_skt_addr.sll_ifindex = tx_if_idx.ifr_ifindex;
	/* Address length*/
	tx_skt_addr.sll_halen = ETH_ALEN;
	/* Destination MAC */
	memcpy(&tx_skt_addr.sll_addr[0], &dest_mac[0], ETH_ALEN);
	return 0;
}

int lts_txskt_send(char *eth_pkt_buf, int eth_pkt_len) {
	struct ether_header *eh = (struct ether_header *) eth_pkt_buf;

	memcpy(&eh->ether_shost[0], &tx_if_mac.ifr_hwaddr.sa_data[0], ETH_ALEN);
	// LOG_INFO("SRC mac: ");
	// PRINT_MAC(&eh->ether_shost[0]);
	memcpy(&eh->ether_dhost[0], &dest_mac[0], ETH_ALEN);
	// LOG_INFO("DST mac: ");
	// PRINT_MAC(&eh->ether_dhost[0]);

	eh->ether_type = htons(ETHER_TYPE);

	if (tx_skt_fd < 0) {
		LOG_ERROR("tx_skt_fd is not open yet");
		return -1;
	}
	if (sendto(tx_skt_fd, eth_pkt_buf, eth_pkt_len, 0, (struct sockaddr*)&tx_skt_addr, 
		sizeof(struct sockaddr_ll)) < 0) {
		LOG_ERROR("Send msg failed");
		return -2;
	}
	return 0;
}