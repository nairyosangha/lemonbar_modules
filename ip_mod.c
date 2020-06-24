#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ev.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <linux/wireless.h>
#include <fcntl.h>

#define MAXLEN (256)

// TODO custom prefix, maybe custom icons?
char output[MAXLEN] = "X ";
char cur_host[MAXLEN]; 
char wifi_icon[] = "\uf1eb "; 
char eth_icon[] = "ETH ";

struct pack {
	char *iface;
	char *gateway;
	int metric;
};


// https://gist.github.com/edufelipe/6108057
int check_wireless(const char* ifname) {
	int sock = -1;
	struct iwreq pwrq;
	memset(&pwrq, 0, sizeof(pwrq));
	strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 0;
	}

	if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
		close(sock);
		return 1;
	}
	close(sock);
	return 0;
}

// TODO I doubt i'll ever use IPv6 on local network, so it probably doesn't really need to support it
// But having a toggle might be nice anyway
int get_IP_for_iface(char *iface) {
	memset(cur_host, '\0', MAXLEN);
	struct ifaddrs *ifaddr, *tmp;
	if (getifaddrs(&ifaddr) == -1) return 0;

	for (tmp = ifaddr; tmp != NULL; tmp = tmp->ifa_next) {
		if (tmp->ifa_name == NULL || strcmp(iface, tmp->ifa_name) != 0) continue;
		if (tmp->ifa_addr->sa_family == AF_INET) {
			getnameinfo(tmp->ifa_addr, sizeof(struct addrinfo), cur_host, sizeof(cur_host), NULL, 0, NI_NUMERICHOST);
			return 1;
		}
	}
	return 0;
}

int gateway_not_NULL(char *gateway)
{
	for (int i = 0; i < (int) strnlen(gateway, MAXLEN); i++)
		// not the cleanest, but it works
		if (gateway[i] > '0') return 1;
	return 0;
}

void get_addresses() {
	memset(output, '\0', MAXLEN);
	output[0] = 'X', output[1] = ' ';

	FILE *f = fopen("/proc/net/route", "r");
	if (!f) return;

	struct pack pack_[2] = {{ "", "", 1000 }, { "", "", 1000}}; // only saving 2, wireless and wired
	char iface[MAXLEN], gateway[MAXLEN], metric[MAXLEN], tmp[MAXLEN];
	fscanf(f, "%s%s%s%s%s%s%s%s%s%s%s\n", iface, tmp, gateway, tmp, tmp, tmp, metric, tmp, tmp, tmp, tmp);
	while(fscanf(f, "%s%s%s%s%s%s%s%s%s%s%s\n", iface, tmp, gateway, tmp, tmp, tmp, metric, tmp, tmp, tmp, tmp) == 11) {
		if (gateway_not_NULL(gateway)) {
			int wifi_bool = check_wireless(iface);
			if (pack_[wifi_bool].metric < atoi(metric)) continue; // saving best metric
			pack_[wifi_bool].iface = strndup(iface, MAXLEN);
			pack_[wifi_bool].gateway = strndup(gateway, MAXLEN);
			pack_[wifi_bool].metric = atoi(metric);
		}
	}
	for (int i = 0; i < 2; i++) {
		//printf("%d: (%s,%s,%d)\tggetwifi? %d\n", i, pack_[i].gateway, pack_[i].iface, pack_[i].metric, check_wireless(pack_[i].iface));
		if (pack_[i].metric == 1000) continue;
		if (strnlen(output, MAXLEN) != 2) strcat(output, " | ");
		strncat(output, i % 2 == 0 ? eth_icon : wifi_icon , MAXLEN);
		strncat(output, get_IP_for_iface(pack_[i].iface) ? cur_host : "NULL", MAXLEN); // get_IP_for_iface saves ret value in global var cur_host
	}

	fclose(f);
	// TODO customize what gets printed if none of the interfaces is up, defaults to wifi for now.
	strnlen(output, MAXLEN) == 2 ? 
		printf("%s%s%s\n", "X ", wifi_icon, "NULL") : printf("%s\n", output);
	fflush(stdout);
}

int main() {
	get_addresses();
	struct ev_loop *loop = EV_DEFAULT;
	ev_stat something;
	ev_stat_init (&something, get_addresses, "/proc/net/route", 0.);
	ev_stat_start (loop, &something);
	ev_run (loop, 0);
	return 1;
}
