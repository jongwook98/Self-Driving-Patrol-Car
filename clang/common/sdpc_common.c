#include <sdpc/common/sdpc_common.h>

ssize_t readn(int fd, void *ptr, size_t n)
{
    size_t nleft = n;

    while (nleft > 0)
    {
        ssize_t nread;

        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (nleft == n)
                return -EPERM;
            else
                break;
        }
        else if (nread == 0)
            break; /* EOF */

        nleft -= nread;
        ptr = (unsigned char *)ptr + nread;
    }

    return (n - nleft);
}

ssize_t writen(int fd, const void *ptr, size_t n)
{
    size_t nleft = n;

    while (nleft > 0)
    {
        ssize_t nwritten;

        if ((nwritten = write(fd, ptr, nleft)) < 0)
        {
            if (nleft == n)
                return -EPERM;
            else
                break;
        }
        else if (nwritten == 0)
            break;

        nleft -= nwritten;
        ptr = (unsigned char *)ptr + nwritten;
    }

    return (n - nleft);
}
