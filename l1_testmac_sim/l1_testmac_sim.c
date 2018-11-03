/*
* @Author: vutang
* @Date:   2018-10-23 09:23:20
* @Last Modified by:   vutang
* @Last Modified time: 2018-11-03 12:36:10
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

#include <getopt.h>
#include <string.h>

#include "logger.h"
#include "fapi_msg.h"
#include "lts_socket.h"

#define LOGDIR "/home/root/l1_testmac_sim.log"
#define ETHER_TYPE	0x5a5b
#define DEFAULT_IF	"eth0"
#define BUF_SIZ		1024

/*Send and Recv Buffer*/
char recvbuf[BUF_SIZ];

/*MAC list:
	b0:83:fe:bb:36:0b: vux's PC eth0
	00:0A:35:00:1E:53: zc706 eth0*/
/*Destination Machine Mac*/
// char dest_mac[6] = {0xb0, 0x83, 0xfe, 0xbb, 0x36, 0x0b};
// char dest_mac[6] = {0xda, 0x02, 0x03, 0x04, 0x05, 0x06};

/*Local interface mac*/
const char eth0_mac[6] = {0x00, 0x0A, 0x35, 0x00, 0x1E, 0x53};
const char eth1_mac[6] = {0x00, 0x0A, 0x35, 0x00, 0x00, 0x01};

int8_t g_dstmac_change = 0, g_test_tx = 0; 
int8_t g_debug_flag = 0;
int rcv_msg_id = -1;
int got_msg = 0;
int wait_time = 1000000;

/*Hold main FAPI State*/
int fapi_state = IDLE;

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

int is_valid_macaddr(char *mac_str, char *mac_arr_ret) {
	uint8_t buf[6] = {0};
	int ret = -1;
	if (strlen(mac_str) != 17) {
		LOG_ERROR("%s too short", mac_str);
		return 0;
	}

	ret = sscanf(mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", &buf[0], \
		&buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);
	if (ret == 6) {
		LOG_DEBUG("Get MAC success");
		memcpy(mac_arr_ret, (char *) &buf[0], 6);
		return 1;
	}
	else {
		printf("%s is invalid mac string", mac_str);
		return 0;
	}
}

void pri_usage(int argc, char **argv) {
	printf("usage: %s --opt [arg]\n", argv[0]);
	printf("\t--help: print usage\n");
	printf("\t--sleep-time/-t <time in usecond>: setting processing time\n");
	printf("\t--debug/-d: printf debug\n");
	printf("\t--dest/-D <MAC Address>; default destmac is da:02:03:04:05:06\n");
	// printf("\t--test/-t\n");
}

void get_args(int argc, char **argv) {
	int c;

	while (1) {
		static struct option long_options[] = {
			{"help", no_argument, 0, 'h'},
			// {"test", no_argument, 0, 't'},
			{"dest", required_argument, 0, 'D'},
			{"debug", no_argument, 0, 'd'},
			{"sleep-time", required_argument, 0, 't'},
			{0, 0, 0, 0}
		};
	      /* getopt_long stores the option index here. */
		int option_index = 0;
		c = getopt_long (argc, argv, "hD:dt:", long_options, &option_index);
		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				pri_usage(argc, argv);
				exit(0);
			// case 't':
			// 	g_test_tx = 1;
			// 	break;
			case 'D':
				printf ("option -d with vl `%s'\n", optarg);
				if (is_valid_macaddr(optarg, dest_mac)) {
					PRINT_MAC(dest_mac);
				}
				else {
					printf("invalid dest mac\n");
					exit(0);
				}

				break;
			case 'd':
				printf("Debug flag\n");
				g_debug_flag = 1;
				break;
			case 't':
				printf("Setup sleep time in FSM, optarg: %s\n", optarg);
				wait_time = strtol(optarg, NULL, 0);
				break;
			case '?':
				/* getopt_long already printed an error message. */
				pri_usage(argc, argv);
				break;
			default:
				pri_usage(argc, argv);
				abort();
		}
	}
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
		if (got_msg == 1) {
			sleep(1);
			continue;
		}
		numbytes = recvfrom(sockfd_rx, recvbuf, BUF_SIZ, 0, NULL, NULL);
		/* Check the packet is for me */
		if (memcmp(&eh->ether_dhost[0], &eth0_mac[0], ETH_ALEN) != 0) 
			continue;
		else {
			rcv_msg_id = -1;
			rcv_msg_id = handle_fapi_msg(recvbuf + sizeof(struct ether_header));
			if (rcv_msg_id < 0)
				continue;
			LOG_DEBUG("got_msg id = %d, %s", rcv_msg_id, get_msg_type_name_(rcv_msg_id));
			got_msg = 1;	
		}
	}

	close(sockfd_rx);
	return;
}

int main (int argc, char *argv[]) {
	int tx_len = 0, i, ether_hdr_len;
	int ret;

	pthread_t rx_thread; /*Hold rx thread*/

	ltePhyL2ApiMsgHdr_t *fapi_hdr = \
		(ltePhyL2ApiMsgHdr_t *) (sendbuf + sizeof(struct ether_header));
	char *ether_payload_ptr = (char *) (sendbuf + sizeof(struct ether_header));

	get_args(argc, argv);

	if (g_debug_flag) {
		LOG_WARN("Turn on log DEBUG Level");
		config_log(LOGDIR, 0x1f, 3);
	}
	else
		config_log(LOGDIR, 0x1e, 3);
	testLog();

	LOG_INFO("Open tx socket");
	ret = lts_txskt_open();
	if (ret < 0) 
		return 1;

	/* Prepare send buf */
	memset(sendbuf, 0, BUF_SIZ);

	if (g_test_tx == 1) {
		ether_hdr_len = sizeof(struct ether_header);
		tx_len += ether_hdr_len;

		for (i = 0; i < 200; i++) {
			*(ether_payload_ptr + i) = (char) i;
			tx_len++;
		}

		for (i = 0; i < 10; i++) {
			LOG_INFO("Send test msg (%d) to: ", tx_len);
			PRINT_MAC(dest_mac);
			ret = lts_txskt_send(sendbuf, tx_len);
			if (ret < 0)
				LOG_WARN("Send MSG Fail");
			sleep(1);
		}
		exit(0);
	}

	LOG_INFO("Open RX Thread");
	pthread_create(&rx_thread, NULL, (void *) &rx_handler, NULL);

	int sent, wait_for_res = 0;

	while (1) {
		tx_len = sizeof(struct ether_header);
		if (fapi_state == IDLE) {
			tx_len += prep_fapi_msg(API_MSG_TYPE_PARAM_REQ);
			if (wait_for_res == 1 && got_msg == 1) {
				if (rcv_msg_id == API_MSG_TYPE_PARAM_RSP) {
					fapi_state = CONFIGURED;
					LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
					wait_for_res = 0;
					continue;
				}
				/*Reset L1 when L2 in IDLE State but L1 keeps sending API_MSG_TYPE_SUBFRAME_IND*/
				else if (rcv_msg_id > 0) {
					fapi_state = RESET_L1;
					LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
					wait_for_res = 0;
					continue;
				}
				got_msg = 0;
			}
		}
		else if (fapi_state == RESET_L1) {
			tx_len += prep_fapi_msg(API_MSG_TYPE_STOP_REQ);
			fapi_state = IDLE;
			LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
			// if (wait_for_res == 1 && got_msg == 1) {
			// 	if (rcv_msg_id == API_MSG_TYPE_STOP_IND) {
			// 		fapi_state = IDLE;
			// 		LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
			// 		wait_for_res = 0;
			// 		continue;
			// 	}
			// 	got_msg = 0;
			// }
			// got_msg = 0;
			continue;
		}
		else if ((fapi_state == CONFIGURED) && (sent != 0)) {
			tx_len += prep_fapi_msg(API_MSG_TYPE_CONFIG_REQ);
			if (wait_for_res == 1 && got_msg == 1) {
				if (rcv_msg_id == API_MSG_TYPE_CONFIG_RSP) {
					fapi_state = RUNNING;
					LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
					wait_for_res = 0;
					continue;
				}
				/*Reset L1 when L2 in IDLE State but L1 keeps sending API_MSG_TYPE_SUBFRAME_IND*/
				else if (rcv_msg_id > 0){
					fapi_state = RESET_L1;
					LOG_INFO("Change state to %s", get_fapi_msg_state_name(fapi_state));
					wait_for_res = 0;
					continue;
				}
				got_msg = 0;
			}
		}
		else if ((fapi_state == RUNNING) && (sent != 0)) {
			tx_len += prep_fapi_msg(API_MSG_TYPE_START_REQ);
			if (wait_for_res == 1 && got_msg == 1) {
				if (rcv_msg_id == API_MSG_TYPE_SUBFRAME_IND) {
					fapi_state = WAIT_INDICATION;
					LOG_INFO("Change state to %d", fapi_state);
					wait_for_res = 0;
					continue;
				}
				got_msg = 0;
			}
		}
		else if ((fapi_state == WAIT_INDICATION)) {
			if ((got_msg == 1) && (rcv_msg_id == API_MSG_TYPE_SUBFRAME_IND)) {
				tx_len += prep_fapi_msg(API_MSG_TYPE_DLCFG_REQ);
				got_msg = 0;
			}
		}
		wait_for_res = 1;
		usleep(wait_time);
	}
	return 0;
}