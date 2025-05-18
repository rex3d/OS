// porty.h - Header file for I/O port operations

#ifndef PORTY_H
#define PORTY_H

// Function prototypes for I/O port operations
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t val);
uint16_t inw(uint16_t port);
void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);
void outb64(uint16_t port, uint8_t val);
uint8_t inb64(uint16_t port);
void outw64(uint16_t port, uint16_t val);
uint16_t inw64(uint16_t port);
void outl64(uint16_t port, uint32_t val);
uint32_t inl64(uint16_t port);

#endif // PORTY_H