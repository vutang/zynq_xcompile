/*
* @Author: vutang
* @Date:   2018-10-23 09:23:20
* @Last Modified by:   vutang
* @Last Modified time: 2018-10-23 09:34:43
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

#include "logger.h"

#define LOGDIR "/home/root/l1_testmac_sim.log"

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

int main () {
	config_log(LOGDIR, 0x1f, 3);
	testLog();
	return 0;
}