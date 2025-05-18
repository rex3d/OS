#ifndef KERNEL_CORE_INIT_H
#define KERNEL_CORE_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file init.h
 * @brief Kernel core initialization interface.
 *
 * This header declares functions for initializing core kernel modules.
 * Extend this file with additional initialization routines as needed.
 */

/**
 * @brief Initializes all core kernel modules.
 *
 * This function should be called at the very beginning of the kernel startup
 * sequence to ensure all essential subsystems are properly initialized.
 */
void kernel_init(void);

/* --- Add additional initialization function declarations below --- */

/**
 * @brief Initializes the memory management subsystem.
 */
void memory_init(void);

/**
 * @brief Initializes the interrupt handling subsystem.
 */
void interrupt_init(void);

/**
 * @brief Initializes the scheduler subsystem.
 */
void scheduler_init(void);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_CORE_INIT_H