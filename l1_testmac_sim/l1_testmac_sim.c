/*
* @Author: vutang
* @Date:   2018-10-23 09:23:20
* @Last Modified by:   vutang
* @Last Modified time: 2018-10-23 11:10:19
*/
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

// #include "ac_ltePhyL2Api.h"
#include "logger.h"
#include "fapi_msg.h"

#define LOGDIR "/home/root/l1_testmac_sim.log"
#define ETHER_TYPE	0x0800
#define DEFAULT_IF	"eth0"
#define BUF_SIZ		1024

#define PRINT_MAC(mac) LOG_INFO("%02x:%02x:%02x:%02x:%02x:%02x", \
		*mac, *(mac+1), *(mac+2), *(mac+3), \
		*(mac+4), *(mac+5));

/*Send and Recv Buffer*/
char sendbuf[BUF_SIZ], recvbuf[BUF_SIZ];


/*MAC list:
	b0:83:fe:bb:36:0b: vux's PC eth0
	00:0A:35:00:1E:53: zc706 eth0*/
/*Destination Machine Mac*/
const char dest_mac[6] = {0xb0, 0x83, 0xfe, 0xbb, 0x36, 0x0b};

/*Local interface mac*/
const char eth0_mac[6] = {0x00, 0x0A, 0x35, 0x00, 0x1E, 0x53};
const char eth1_mac[6] = {0x00, 0x0A, 0x35, 0x00, 0x00, 0x01};

void testLog() {
	printf("Test LOG configuration\n");
	LOG_WARN("LOG_WARN");
	LOG_DEBUG("LOG_DEBUG");
	LOG_INFO("LOG_INFO");
	LOG_ERROR("LOG_ERROR");
	LOG_FATAL("LOG_FATAL");
}

void software_header(void) {
	printf("L2 FAPI Simulator\n");
	printf("Build date: %s\t%s\n", __DATE__, __TIME__);
	printf("--------------------------\n");
}

void rx_handler(void* arg) {
	int sockfd_rx;
	int ret, i, sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct sockaddr_storage their_addr;
	uint8_t recvbuf[BUF_SIZ];
	char ifName[IFNAMSIZ];

	strcpy(ifName, DEFAULT_IF);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) recvbuf;

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd_rx = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		return;
	}

	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd_rx, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd_rx, SIOCSIFFLAGS, &ifopts);

	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd_rx, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd_rx);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd_rx, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd_rx);
		exit(EXIT_FAILURE);
	}

	while (1) {
		numbytes = recvfrom(sockfd_rx, recvbuf, BUF_SIZ, 0, NULL, NULL);
		/* Check the packet is for me */
		if (memcmp(&eh->ether_dhost[0], &eth0_mac[0], ETH_ALEN) != 0) 
			continue;
		else {
			LOG_INFO("got_msg ...");
		}
	}

	close(sockfd_rx);
	return;
}

int main (int argc, char *argv[]) {
	/*Socket file description*/
	int sockfd_tx;
	/*Ethernet header*/
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct sockaddr_ll socket_address;
	struct ifreq tx_if_idx, tx_if_mac;

	char ifName[IFNAMSIZ];

	int tx_len;

	pthread_t rx_thread; /*Hold rx thread*/

	config_log(LOGDIR, 0x1f, 3);
	testLog();

	/* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);

	/* Open RAW socket to send on */
	if ((sockfd_tx = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&tx_if_idx, 0, sizeof(struct ifreq));
	strncpy(tx_if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd_tx, SIOCGIFINDEX, &tx_if_idx) < 0)
	    perror("SIOCGIFINDEX");

	/* Get the MAC address of the interface to send on */
	memset(&tx_if_mac, 0, sizeof(struct ifreq));
	strncpy(tx_if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd_tx, SIOCGIFHWADDR, &tx_if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	memcpy(&eh->ether_shost[0], &tx_if_mac.ifr_hwaddr.sa_data[0], ETH_ALEN);
	LOG_INFO("SRC mac: ");
	PRINT_MAC(&eh->ether_shost[0]);
	memcpy(&eh->ether_dhost[0], &dest_mac[0], ETH_ALEN);
	LOG_INFO("DST mac: ");
	PRINT_MAC(&eh->ether_dhost[0]);

	/* Ethertype field */
	// eh->ether_type = htons(ETH_P_IP);
	eh->ether_type = htons(0xF5F5);

	/* Index of the network device */
	socket_address.sll_ifindex = tx_if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	memcpy(&socket_address.sll_addr[0], &dest_mac[0], ETH_ALEN);
	/* Send packet */

	LOG_INFO("Open RX Thread");
	pthread_create(&rx_thread, NULL, (void *) &rx_handler, NULL);

	while (1) {
		tx_len = sizeof(struct ether_header);
		tx_len += prep_fapi_msg(sendbuf + sizeof(struct ether_header), API_MSG_TYPE_PARAM_REQ);
		LOG_INFO("Send msg to: %d", tx_len);
		PRINT_MAC(dest_mac);
		if (sendto(sockfd_tx, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, 
			sizeof(struct sockaddr_ll)) < 0) {
			LOG_ERROR("Send msg failed");
		}
		sleep(1);
	}
	return 0;
}