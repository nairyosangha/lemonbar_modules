#include <stdio.h>
#include <time.h>
#include <ev.h>

#define MAX_SIZE 64
#define TIMER_TIME 60.

time_t rawtime;
ev_timer timer;

int seconds_til_minute() {
	time(&rawtime);
	return 60 - localtime(&rawtime)->tm_sec;
}

void print_time() {
	char output[MAX_SIZE];
	time(&rawtime);
	strftime(output, MAX_SIZE, "S%a %e/%m - %H:%M", localtime(&rawtime));
	printf("%s\n", output);
	fflush(stdout);
}

// callback function, called when timer reaches 0
static void timer_cb(EV_P_ ev_timer *timer, int r)
{
	ev_timer_stop (loop, timer);
	ev_timer_set (timer, TIMER_TIME, 0.);
	ev_timer_start (loop, timer);
	print_time();
}

int main() 
{
	unsigned int offset = seconds_til_minute();
	print_time();
	struct ev_loop *loop = EV_DEFAULT;
	ev_timer_init (&timer, timer_cb, offset, 0.);
	ev_timer_start (loop, &timer);
	ev_run(loop, 0);
	return 0; // we really shouldn't ever exit unless the bar is killed
}
