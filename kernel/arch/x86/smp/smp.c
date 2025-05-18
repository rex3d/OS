#include "smp.h"
#include "../apic/lapic.h"

// Liczba wykrytych CPU (przynajmniej jeden - BSP)
static int cpu_count = 1;
static int smp_initialized = 0;

// Szkielet funkcji wykrywającej liczbę CPU (np. przez ACPI/MADT)
static int detect_cpu_count(void) {
    // TODO: Zaimplementuj wykrywanie CPU przez ACPI/MADT lub MP Table
    // Na razie zwracamy 1 (tylko BSP)
    return 1;
}

void smp_init(void) {
    if (smp_initialized)
        return;
    cpu_count = detect_cpu_count();
    smp_initialized = 1;
}

void smp_start_aps(void) {
    // Uruchom Application Processors (AP)
    for (int apic_id = 1; apic_id < cpu_count; ++apic_id) {
        lapic_send_ipi(apic_id, INIT_VECTOR);
        lapic_send_ipi(apic_id, SIPI_VECTOR);
        lapic_send_ipi(apic_id, SIPI_VECTOR);
        // Można dodać opóźnienia i sprawdzanie statusu AP
    }
}

int smp_get_cpu_count(void) {
    return cpu_count;
}