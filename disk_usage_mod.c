#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ev.h>
#include <sys/vfs.h>
// these are used by the commented out code in main
// #include <fcntl.h>
// #include <unistd.h>

#define MAX_LEN (512)

enum formatting { FREE = 0x01, USED = 0x02, TOTAL = 0x04 };
struct statfs *buf = NULL;
static char *size_fmt[] = { "B", "KB", "MB", "GB", "TB", NULL };
static char *blacklist_mnttypes[] = { "tmpfs", "cgroup", "proc", "sysfs", "devtmpfs", "efivarfs", "securityfs", "devpts", "cgroup2", "pstore", "bpf", "autofs", "mqueue", "hugetlbfs", "debugfs", "tracefs", "binfmt_misc", "configfs", "fuse.gvfsd-fuse", "fusectl", NULL };
char disk_icon[] = "\uf0c7";

int approved_mnttype(char *mount_type, int len) 
{
	for (int i = 0; blacklist_mnttypes[i] != NULL; i++) {
		if (strncmp(mount_type, blacklist_mnttypes[i], len) == 0) return 0;
	}
	return 1;
}

void blocks_to_readable_bytesize(double *converted_bytes, int *fmt_index, unsigned long total_bytes) 
{
	int scale = ceil(log10(total_bytes)); // rounded up amount of digits
	scale += scale % 3 == 0 ? 0 : (3 - scale % 3); // conv amt of digits to a power of 3, so we can determine whether dealing with MB, GB, TB
	scale /= 3;
	*fmt_index = scale - 1;
	*converted_bytes = total_bytes / pow(1024, scale - 1);
}

void get_sizes(unsigned int output) 
{
	FILE *f = fopen("/proc/self/mounts", "r");
	if (!f) return;

	unsigned long total_bytes = 0, bytes_used = 0, bytes_free = 0;
	char mount[MAX_LEN], mount_point[MAX_LEN], mount_type[MAX_LEN];
	char mount_opts[MAX_LEN]; int a, b; // i dont care about these, they're kept to make fscanf work nicely
	while (fscanf(f, "%s %s %s %s %d %d", mount, mount_point, mount_type, mount_opts, &a, &b) == 6) {
		if (approved_mnttype(mount_type, 512)) {
			statfs(mount_point, buf);
			total_bytes += buf->f_blocks * buf->f_bsize;
			bytes_free += buf->f_bavail * buf->f_bsize;
			memset(buf, 0, sizeof(struct statfs));
		}
	}
	bytes_used = total_bytes - bytes_free;
	
	double converted_bytes; int identifier;
	if (output & FREE) {
		blocks_to_readable_bytesize(&converted_bytes, &identifier, bytes_free);
		printf("D %s %.2f %s free\n", disk_icon, converted_bytes, size_fmt[identifier]);
	}
	if (output & USED) {
		blocks_to_readable_bytesize(&converted_bytes, &identifier, bytes_used);
		printf("D%.2f %s used\n", converted_bytes, size_fmt[identifier]);
	}
	if (output & TOTAL) {
		blocks_to_readable_bytesize(&converted_bytes, &identifier, total_bytes);
		printf("D%.2f %s total\n", converted_bytes, size_fmt[identifier]);
	}
	fflush(stdout);
	fclose(f);
}

void get_free_sizes() {	get_sizes(FREE); }

void handle_sigint()
{
	free(buf);
	exit(1);
}

int main() {
	signal(SIGINT, handle_sigint);
	buf = malloc(sizeof(struct statfs));

	get_sizes(FREE);
	struct ev_loop *loop = EV_DEFAULT;
	ev_timer timer;
	ev_init(&timer, get_free_sizes);
	timer.repeat = 30.;
	ev_timer_again(loop, &timer);
	ev_run(loop, 0);


	// this gets spammed way too much!
	//int fd = open("/proc/diskstats", O_RDONLY);
	//struct ev_loop *loop = EV_DEFAULT;
	//ev_io something;
	//ev_io_init (&something, get_sizes, fd, EV_WRITE);
	////ev_stat_init (&something, get_sizes, "/proc/diskstats", 0.);
	//ev_io_start (loop, &something);
	//ev_run (loop, 0);
	//close(fd);
	

	// this never activates!
	//struct ev_loop *loop = EV_DEFAULT;
	//ev_stat timer;
	//ev_stat_init(&timer, get_sizes, "/proc/diskstats", 0.);
	//ev_stat_start(loop, &timer);
	//ev_run(loop, 0);

	return 0;
}
