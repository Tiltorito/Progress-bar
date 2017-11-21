#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
    char ch;                            // char which will be used to display the bar
    unsigned int size;                  // size for the bar (don't have to be the same as max_value)
    unsigned int max_value;
    unsigned int current_value;
    struct timeval* time_started;        // this time_t gets initialized on the first increase
    struct timeval* prev_increase;       // this is used to find the freq between increases(to estimate when it will finish)
    double freq;                        // the actually frequancy
} progress_bar;

/**
 * returns the estimated finish time for the progress bar in seconds.
 * @param bar the progress_bar
 * @param value the value of the increase
 * @return
 */
int progress_bar_estimate_finishTime(progress_bar* bar, unsigned int value) {
    unsigned int rest = (bar->max_value - bar->current_value);
    unsigned int seconds;

    if(value == 0) {
        seconds = 0xFFFFFFFF;
    }
    else {
        seconds = rest * bar->freq / value;
    }

    return seconds;
}

int progress_bar_timeRunning(progress_bar* bar) {
    struct timeval now;
    gettimeofday(&now, NULL);


    struct timeval res;
    timersub(&now, bar->time_started, &res);

    return (int)res.tv_sec;
}

void progress_bar_print_bar(progress_bar* bar) {
    int percentage = bar->current_value * bar->size / bar->max_value;

    putchar('[');

    int i;

    for(i = 0; i < percentage; i++) {
        putchar(bar->ch);
    }

    for(; i < bar->size; i++) {
        putchar(' ');
    }

    putchar(']');
}

void progress_bar_print(progress_bar* bar, unsigned int value) {
    if(bar == NULL) {
        return;
    }

    putchar('\r');

    progress_bar_print_bar(bar);

    putchar(' ');

    printf("[%d/%d]", bar->current_value, bar->max_value); // prints [current_value/max_value]

    putchar(' ');

    int seconds = progress_bar_estimate_finishTime(bar, value); // getting estimated remaining seconds
    int minutes = seconds / 60;
    seconds %= 60;

    printf("[%02d:%02d]", minutes, seconds);

    putchar(' ');

    int time_running = progress_bar_timeRunning(bar);

    printf("[%02ds]", time_running);

    fflush(stdout);
}

progress_bar* make_progress_bar(unsigned int max_value, char ch, unsigned int start_value, unsigned int size) {
    progress_bar* bar = (progress_bar*) malloc(sizeof(progress_bar));

    bar->max_value = max_value;
    bar->ch = ch;
    bar->current_value = start_value;
    bar->size = size;
    bar->time_started = NULL;
    bar->prev_increase = NULL;
    bar->freq = 0;

    return bar;
}

progress_bar* make_progress_bar_default() {
    return make_progress_bar(100, '#', 0, 50);
}

int progress_bar_increase(progress_bar* bar, unsigned int value) {
    if(bar == NULL || bar->max_value == bar->current_value) {
        return 0;
    }

    // if its the first increase, allocate memory for the time_started timeval, and init it.
    if(bar->time_started == NULL) {
        bar->time_started = (struct timeval*)malloc(sizeof(struct timeval));
        gettimeofday(bar->time_started, NULL);
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    // if we have prev increase find the difference and store it in bar->freq.
    if(bar->prev_increase != NULL) {
        struct timeval difference;
        timersub(&now, bar->prev_increase, &difference);
        bar->freq = (int)difference.tv_sec + ((double)difference.tv_usec) / 1000 / 1000;
    }
    else {
        bar->prev_increase = (struct timeval*) malloc(sizeof(struct timeval));
    }

    // prev_increase becoming the current time
    *(bar->prev_increase) = now;

    bar->current_value += value;

    // A simple check to not go further than max_value
    if(bar->current_value > bar->max_value) {
        bar->current_value = bar->max_value;
    }

    progress_bar_print(bar, value);

    return 1;
}

int progress_bar_close(progress_bar* bar) {
    if(bar == NULL) {
        return 0;
    }

    // free the memory for the 2 pointer, which they pointed to struct timeval
    if(bar->time_started != NULL) {
        free(bar->time_started);
    }

    if(bar->prev_increase != NULL) {
        free(bar->prev_increase);
    }

    free(bar);

    return 1;
}