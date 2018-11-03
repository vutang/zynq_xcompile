#define PRINT_MAC(mac) LOG_INFO("%02x:%02x:%02x:%02x:%02x:%02x", \
		*mac, *(mac+1), *(mac+2), *(mac+3), \
		*(mac+4), *(mac+5));
int lts_txskt_open();
int lts_txskt_send(char *eth_pkt_buf, int eth_pkt_len);