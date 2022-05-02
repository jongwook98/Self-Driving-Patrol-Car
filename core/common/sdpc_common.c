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

sync_t *sync_create(void)
{
    sync_t *new = malloc(sizeof(sync_t));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    pthread_cond_init(&new->cv, NULL);
    pthread_mutex_init(&new->lock, NULL);

    return new;
}

int sync_destroy(sync_t **const sync)
{
    FORMULA_GUARD(sync == NULL || *sync == NULL, -EPERM, ERR_INVALID_PTR);

    sync_t *del = *sync;

    pthread_cond_destroy(&del->cv);
    pthread_mutex_destroy(&del->lock);

    PTR_FREE(del);

    return 0;
}
