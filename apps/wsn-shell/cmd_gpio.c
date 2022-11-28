#include <stdlib.h>
#include <string.h>

#include <periph/gpio.h>


static int print_usage(void)
{
    puts("usage: gpio [init_in|read] <port> <pin>");
    puts("usage: gpio [init_out|set|clear|toggle] <port> <pin>");
    return 1;
}

int cmd_gpio(int argc, char **argv)
{
    if (argc != 4) {
        return print_usage();
    }

    char *cmd = argv[1];
    int port = atoi(argv[2]);
    int pin = atoi(argv[3]);
    gpio_t gpio = GPIO_PIN(port, pin);
    int err = 0;

    if (strcmp(cmd, "init_out") == 0) {
        err = gpio_init(gpio, GPIO_OUT);
        if (err) {
            printf("Error to initialize GPIO_PIN(%i, %02i)\n", port, pin);
        }
        return err;
    }
    else if (strcmp(cmd, "init_in") == 0) {
        err = gpio_init(gpio, GPIO_OUT);
        if (err) {
            printf("Error to initialize GPIO_PIN(%i, %02i)\n", port, pin);
        }
        return err;
    }
    else if (strcmp(cmd, "read") == 0) {
        if (gpio_read(GPIO_PIN(port, pin))) {
            printf("GPIO_PIN(%i.%02i) is HIGH\n", port, pin);
        }
        else {
            printf("GPIO_PIN(%i.%02i) is LOW\n", port, pin);
        }
        return 0;
    }
    else if (strcmp(cmd, "set") == 0) {
        gpio_set(gpio);
        return 0;
    }
    else if (strcmp(cmd, "clear") == 0) {
        gpio_clear(gpio);
        return 0;
    }
    else if (strcmp(cmd, "toggle") == 0) {
        gpio_toggle(gpio);
        return 0;
    }

    return print_usage();
}
