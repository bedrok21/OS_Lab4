#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#define NUM_KEYCODES 71

const char *keycodes[] = {
    "RESERVED",
    "ESC",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "MINUS",
    "EQUAL",
    "BACKSPACE",
    "TAB",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "LEFTBRACE",
    "RIGHTBRACE",
    "ENTER",
    "LEFTCTRL",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    "SEMICOLON",
    "APOSTROPHE",
    "GRAVE",
    "LEFTSHIFT",
    "BACKSLASH",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    "COMMA",
    "DOT",
    "SLASH",
    "RIGHTSHIFT",
    "KPASTERISK",
    "LEFTALT",
    "SPACE",
    "CAPSLOCK",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "NUMLOCK",
    "SCROLLLOCK"
};

int main() {
    int fd;
    struct input_event ev;

    fd = open("/dev/input/event0", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        read(fd, &ev, sizeof(struct input_event));

        if (ev.type == EV_KEY && ev.value == 1) {
            if(ev.code > 0 && ev.code < NUM_KEYCODES){
                printf("%s\n", keycodes[ev.code]);
            }
            else{
                printf("UNRECOGNIZED\n");
            }
        }

    }

    close(fd);
    return 0;
}
