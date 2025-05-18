#ifndef KERNEL_H
#define KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

// Deklaracja funkcji głównej kernela
void kernel_main(void);

// Makro do logowania (implementacja zależna od architektury)
#if defined(__x86_64__) || defined(_M_X64)
    #define KERNEL_64BIT
#endif

#ifdef KERNEL_64BIT
    #define KERNEL_LOG(msg) /* TODO: Dodaj obsługę logowania na VGA/serial */
#else
    #define KERNEL_LOG(msg) video_print(msg)
#endif

#ifdef __cplusplus
}
#endif

#endif // KERNEL_H