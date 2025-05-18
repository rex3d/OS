#include "devfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
typedef CRITICAL_SECTION devfs_mutex_t;
#define DEVFS_MUTEX_INIT(m) InitializeCriticalSection(&(m))
#define DEVFS_MUTEX_LOCK(m) EnterCriticalSection(&(m))
#define DEVFS_MUTEX_UNLOCK(m) LeaveCriticalSection(&(m))
#define DEVFS_MUTEX_DESTROY(m) DeleteCriticalSection(&(m))
#else
#include <pthread.h>
typedef pthread_mutex_t devfs_mutex_t;
#define DEVFS_MUTEX_INIT(m) pthread_mutex_init(&(m), NULL)
#define DEVFS_MUTEX_LOCK(m) pthread_mutex_lock(&(m))
#define DEVFS_MUTEX_UNLOCK(m) pthread_mutex_unlock(&(m))
#define DEVFS_MUTEX_DESTROY(m) pthread_mutex_destroy(&(m))
#endif

static devfs_device_t* devfs_device_list = NULL;
static devfs_mutex_t devfs_mutex;

void devfs_init(void) {
    devfs_device_list = NULL;
    DEVFS_MUTEX_INIT(devfs_mutex);
}

void devfs_cleanup(void) {
    DEVFS_MUTEX_LOCK(devfs_mutex);
    devfs_device_t* dev = devfs_device_list;
    while (dev) {
        devfs_device_t* next = dev->next;
        free((void*)dev->name);
        free(dev);
        dev = next;
    }
    devfs_device_list = NULL;
    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    DEVFS_MUTEX_DESTROY(devfs_mutex);
}

int devfs_register(const char* name,
                   devfs_read_fn read,
                   devfs_write_fn write,
                   devfs_ioctl_fn ioctl) {
    if (!name || !read || !write || !ioctl)
        return -EINVAL;

    devfs_device_t* dev = malloc(sizeof(devfs_device_t));
    if (!dev) return -ENOMEM;

    dev->name = strdup(name);
    if (!dev->name) {
        free(dev);
        return -ENOMEM;
    }

    dev->read = read;
    dev->write = write;
    dev->ioctl = ioctl;

    DEVFS_MUTEX_LOCK(devfs_mutex);
    dev->next = devfs_device_list;
    devfs_device_list = dev;
    DEVFS_MUTEX_UNLOCK(devfs_mutex);

    return 0;
}

int devfs_unregister(const char* name) {
    if (!name)
        return -EINVAL;

    DEVFS_MUTEX_LOCK(devfs_mutex);
    devfs_device_t **prev = &devfs_device_list, *dev = devfs_device_list;

    while (dev) {
        if (strcmp(dev->name, name) == 0) {
            *prev = dev->next;
            free((void*)dev->name);
            free(dev);
            DEVFS_MUTEX_UNLOCK(devfs_mutex);
            return 0;
        }
        prev = &dev->next;
        dev = dev->next;
    }

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return -ENOENT;
}

devfs_device_t* devfs_open(const char* name) {
    if (!name)
        return NULL;

    DEVFS_MUTEX_LOCK(devfs_mutex);
    devfs_device_t* dev = devfs_device_list;

    while (dev) {
        if (strcmp(dev->name, name) == 0) {
            DEVFS_MUTEX_UNLOCK(devfs_mutex);
            return dev;
        }
        dev = dev->next;
    }

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return NULL;
}

int devfs_read(devfs_device_t* dev, void* buf, size_t count, size_t* read_count) {
    if (!dev || !buf || !read_count || !dev->read)
        return -EINVAL;
    return dev->read(buf, count, read_count);
}

int devfs_write(devfs_device_t* dev, const void* buf, size_t count, size_t* written_count) {
    if (!dev || !buf || !written_count || !dev->write)
        return -EINVAL;
    return dev->write(buf, count, written_count);
}

int devfs_ioctl(devfs_device_t* dev, int cmd, void* arg) {
    if (!dev || !dev->ioctl)
        return -EINVAL;
    return dev->ioctl(cmd, arg);
}

void devfs_list_devices(void) {
    DEVFS_MUTEX_LOCK(devfs_mutex);
    devfs_device_t* dev = devfs_device_list;
    printf("Registered devices:\n");
    while (dev) {
        printf(" - %s\n", dev->name);
        dev = dev->next;
    }
    DEVFS_MUTEX_UNLOCK(devfs_mutex);
}
