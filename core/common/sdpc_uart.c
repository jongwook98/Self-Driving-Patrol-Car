#include <termios.h>

#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_uart.h>

#define UART_PATH "/dev/ttyUSB0"

struct sdpc_uart
{
    int fd;
    char buf[MAX_BUF];

    pthread_mutex_t lock;
};

static struct sdpc_uart *_uart_create(void);
static int _uart_destroy(struct sdpc_uart *del);

static int _uart_open(struct sdpc_uart *uart, const char *path);
static int _uart_close(struct sdpc_uart *uart);
static int _uart_set_attributs(int fd, int speed, int parity);

static inline ssize_t _uart_send(struct sdpc_uart *uart);
static inline ssize_t _uart_receive(struct sdpc_uart *uart);

static struct sdpc_uart *g_uart;

int sdpc_uart_init(void)
{
    FORMULA_GUARD(g_uart, -EPERM, ERR_INVALID_PTR);

    g_uart = _uart_create();
    FORMULA_GUARD(g_uart == NULL, -EPERM, ERR_INVALID_PTR);

    return 0;
}

int sdpc_uart_exit(void)
{
    FORMULA_GUARD(g_uart == NULL, -EPERM, ERR_INVALID_PTR);

    _uart_destroy(g_uart);
    g_uart = NULL;

    return 0;
}

int sdpc_uart_send(const char *send_msg)
{
    FORMULA_GUARD(g_uart == NULL || send_msg == NULL, -EPERM, ERR_INVALID_PTR);

    ssize_t len = strlen(send_msg);
    FORMULA_GUARD(len == 0, -EPERM, "Empty message,,");

    struct sdpc_uart *uart = g_uart;

    pthread_mutex_lock(&uart->lock);
    snprintf(uart->buf, sizeof(uart->buf), "%s", send_msg);
    ssize_t ret = _uart_send(uart);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret != len, -EPERM,
                                 pthread_mutex_unlock(&uart->lock),
                                 "Failed to send the message!");
    pthread_mutex_unlock(&uart->lock);
    return 0;
}

int sdpc_uart_receive(char *read_buf)
{
    FORMULA_GUARD(g_uart == NULL || read_buf == NULL, -EPERM, ERR_INVALID_PTR);

    struct sdpc_uart *uart = g_uart;

    pthread_mutex_lock(&uart->lock);
    ssize_t ret = _uart_receive(uart);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret <= 0, -EPERM,
                                 pthread_mutex_unlock(&uart->lock),
                                 "Failed to receive the message!");
    snprintf(read_buf, strlen(uart->buf) + 1, "%s", uart->buf);
    pthread_mutex_unlock(&uart->lock);
    return 0;
}

static struct sdpc_uart *_uart_create(void)
{
    struct sdpc_uart *new = calloc(1, sizeof(struct sdpc_uart));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new->fd = -EPERM;
    pthread_mutex_init(&new->lock, NULL);

    int ret = _uart_open(new, UART_PATH);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, NULL, _uart_destroy(new), "");

    return new;
}

static int _uart_destroy(struct sdpc_uart *del)
{
    _uart_close(del);

    pthread_mutex_destroy(&del->lock);
    PTR_FREE(del);

    return 0;
}

static int _uart_open(struct sdpc_uart *uart, const char *path)
{
    FORMULA_GUARD(uart == NULL || path == NULL, -EPERM, ERR_INVALID_PTR);

    uart->fd = open(path, O_RDWR | O_NOCTTY | O_SYNC);
    FORMULA_GUARD(uart->fd < 0, -EPERM, ERR_INVALID_FD);

    int ret = _uart_set_attributs(uart->fd, B115200, 0);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM, _uart_close(uart), "");

    return 0;
}

static int _uart_close(struct sdpc_uart *uart)
{
    FORMULA_GUARD(uart == NULL, -EPERM, ERR_INVALID_PTR);

    if (uart->fd > 0)
        close(uart->fd);

    uart->fd = -EPERM;
    return 0;
}

static int _uart_set_attributs(int fd, int speed, int parity)
{
    struct termios tty;
    memset((void *)&tty, 0x00, sizeof(struct termios));

    int ret = tcgetattr(fd, &tty);
    FORMULA_GUARD(ret, -EPERM, "Failed to get attributes. %d", ret);

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    /* 8bit char */
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

    /*
     * Disable IGNBRK for mismatched speed tests;
     * Otherwise receive break as \000 chars
     */
    tty.c_iflag &= (~IGNBRK);

    /* No signaling chars, no echo, no canonical processing */
    tty.c_lflag = 0;

    /* No remapping, no delays */
    tty.c_oflag = 0;

    /* Read doesn't block */
    tty.c_cc[VMIN] = 0;

    /* 0.5 seconds read timeout */
    tty.c_cc[VTIME] = 5;

    /* shut off xon/xoff ctrl */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Ignore modem controls, enable reading */
    tty.c_cflag |= (CLOCAL | CREAD);

    /* shut off parity */
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag |= parity;
    tty.c_cflag &= ~(CSTOPB);
    tty.c_cflag &= ~(CRTSCTS);

    ret = tcsetattr(fd, TCSANOW, &tty);
    FORMULA_GUARD(ret, -EPERM, "Failed to set attributes. %d", ret);

    return 0;
}

static inline ssize_t _uart_send(struct sdpc_uart *uart)
{
    return writen(uart->fd, uart->buf, strlen(uart->buf));
}

static inline ssize_t _uart_receive(struct sdpc_uart *uart)
{
    return readn(uart->fd, uart->buf, sizeof(uart->buf));
}
