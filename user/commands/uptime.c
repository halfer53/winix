#include <sys/syscall.h>
#include <sys/times.h>
#include <sys/unistd.h>
/**
 * Prints the system uptime
 **/
int main(int argc, char **argv) {
    // char out[] = "\e[1;1H\e[2J";

    int ticks, days, hours, minutes, seconds, tick_rate;
    struct tms tbuf;
    ticks = times(&tbuf);
    tick_rate = sysconf(_SC_CLK_TCK);
    seconds = ticks / tick_rate; 
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;

    seconds %= 60;
    minutes %= 60;
    hours %= 24;

    printf("Uptime is %dd %dh %dm %d.%02ds\n", days, hours, minutes, seconds, ticks % tick_rate);
    printf("user time %d.%d seconds, system time %d.%02d seconds\n",
                            tbuf.tms_utime  / tick_rate , (tbuf.tms_utime) % tick_rate,
                            tbuf.tms_stime / tick_rate , (tbuf.tms_stime) % tick_rate);
    
    return 0;
}

