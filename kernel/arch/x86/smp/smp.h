#ifndef SMP_H
#define SMP_H

#include <stdint.h>

// Initializes SMP (Symmetric Multiprocessing) support
void smp_init(void);

// Starts Application Processors (APs)
void smp_start_aps(void);

// Returns the number of CPUs detected
int smp_get_cpu_count(void);

// Returns the current CPU's ID
int smp_get_current_cpu_id(void);

// Sends an Inter-Processor Interrupt (IPI) to a specific CPU
void smp_send_ipi(int cpu_id, uint8_t vector);

#endif // SMP_H