# Progress-bar

## Client code

```int main() {
    progress_bar* bar = make_progress_bar(100, '=', 0, 65);
    int i;
    for(i = 0; i < 100; i++) {
        usleep(100000);
        progress_bar_increase(bar, 1);
    }

    progress_bar_close(bar);
}```

## API

### Call this function to make a new progress_bar
```progress_bar* make_progress_bar(unsigned int max_value, char ch, unsigned int start_value, unsigned int size);```

### Call this function to increase the value of the progress_bar
```int progress_bar_increase(progress_bar* bar, unsigned int value);```

### Call this function to free the memory of the progress_bar
```int progress_bar_close(progress_bar* bar);```