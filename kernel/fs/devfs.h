#include "devfs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    DEVFS_MUTEX_INIT(devfs_mutex);
    devfs_device_list = NULL;
}

int devfs_register(const char* name,
                   devfs_device_type_t type,
                   uint32_t flags,
                   int (*read)(devfs_device_t*, void*, size_t, size_t*),
                   int (*write)(devfs_device_t*, const void*, size_t, size_t*),
                   int (*ioctl)(devfs_device_t*, int, void*),
                   int (*close)(devfs_device_t*),
                   devfs_event_cb_t event_cb,
                   void* user_data) {
    if (!name || strlen(name) >= DEVFS_MAX_NAME_LEN)
        return DEVFS_ERR_INVAL;

    DEVFS_MUTEX_LOCK(devfs_mutex);

    devfs_device_t* existing = devfs_device_list;
    while (existing) {
        if (strcmp(existing->name, name) == 0) {
            DEVFS_MUTEX_UNLOCK(devfs_mutex);
            return DEVFS_ERR_EXIST;
        }
        existing = existing->next;
    }

    devfs_device_t* dev = (devfs_device_t*)calloc(1, sizeof(devfs_device_t));
    if (!dev) {
        DEVFS_MUTEX_UNLOCK(devfs_mutex);
        return DEVFS_ERR_NOMEM;
    }

    strncpy(dev->name, name, DEVFS_MAX_NAME_LEN - 1);
    dev->type = type;
    dev->flags = flags;
    dev->refcount = 0;
    dev->read = read;
    dev->write = write;
    dev->ioctl = ioctl;
    dev->close = close;
    dev->event_cb = event_cb;
    dev->user_data = user_data;
    dev->next = devfs_device_list;
    devfs_device_list = dev;

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return DEVFS_OK;
}

int devfs_unregister(const char* name) {
    if (!name)
        return DEVFS_ERR_INVAL;

    DEVFS_MUTEX_LOCK(devfs_mutex);

    devfs_device_t **prev = &devfs_device_list;
    devfs_device_t *curr = devfs_device_list;

    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (curr->refcount > 0) {
                DEVFS_MUTEX_UNLOCK(devfs_mutex);
                return DEVFS_ERR_BUSY;
            }

            *prev = curr->next;
            free(curr);
            DEVFS_MUTEX_UNLOCK(devfs_mutex);
            return DEVFS_OK;
        }
        prev = &curr->next;
        curr = curr->next;
    }

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return DEVFS_ERR_NOENT;
}

devfs_device_t* devfs_open(const char* name) {
    if (!name)
        return NULL;

    DEVFS_MUTEX_LOCK(devfs_mutex);

    devfs_device_t* dev = devfs_device_list;
    while (dev) {
        if (strcmp(dev->name, name) == 0) {
            dev->refcount++;
            DEVFS_MUTEX_UNLOCK(devfs_mutex);
            return dev;
        }
        dev = dev->next;
    }

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return NULL;
}

int devfs_close(devfs_device_t* dev) {
    if (!dev)
        return DEVFS_ERR_INVAL;

    DEVFS_MUTEX_LOCK(devfs_mutex);
    if (dev->refcount > 0)
        dev->refcount--;

    if (dev->refcount == 0 && dev->close) {
        int ret = dev->close(dev);
        DEVFS_MUTEX_UNLOCK(devfs_mutex);
        return ret;
    }

    DEVFS_MUTEX_UNLOCK(devfs_mutex);
    return DEVFS_OK;
}

int devfs_read(devfs_device_t* dev, void* buf, size_t count, size_t* read_count) {
    if (dev && dev->read)
        return dev->read(dev, buf, count, read_count);
    return DEVFS_ERR_INVAL;
}

int devfs_write(devfs_device_t* dev, const void* buf, size_t count, size_t* written_count) {
    if (dev && dev->write)
        return dev->write(dev, buf, count, written_count);
    return DEVFS_ERR_INVAL;
}

int devfs_ioctl(devfs_device_t* dev, int cmd, void* arg) {
    if (dev && dev->ioctl)
        return dev->ioctl(dev, cmd, arg);
    return DEVFS_ERR_INVAL;
}

const char* devfs_get_name(const devfs_device_t* dev) {
    return dev ? dev->name : NULL;
}

devfs_device_type_t devfs_get_type(const devfs_device_t* dev) {
    return dev ? dev->type : DEVFS_TYPE_UNKNOWN;
}

uint32_t devfs_get_flags(const devfs_device_t* dev) {
    return dev ? dev->flags : 0;
}

int devfs_get_refcount(const devfs_device_t* dev) {
    return dev ? dev->refcount : -1;
}

void devfs_enum_devices(devfs_enum_cb_t cb, void* user_data) {
    if (!cb) return;

    DEVFS_MUTEX_LOCK(devfs_mutex);
    devfs_device_t* dev = devfs_device_list;
    while (dev) {
        cb(dev, user_data);
        dev = dev->next;
    }
    DEVFS_MUTEX_UNLOCK(devfs_mutex);
}
