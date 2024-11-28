/**
 * @file Serial.cpp
 * @brief Support code for USART0,1,2,3
 * @author jzupan 
 * @date 2023/24
 */
#include <avr/io.h>
#include <util/delay.h>

/* info:
void portmux_init_0(void) { // Adjust PORTMUX for USART modules:
    PORTMUX.USARTROUTEA = PORTMUX_USART0_DEFAULT_gc  // USART0 na PA0/PA1 (default tx/rx)
                        | PORTMUX_USART1_ALT1_gc     // USART1 na PC4/PC5
                        | PORTMUX_USART2_DEFAULT_gc  // USART2 na PF0/PF1 (default tx/rx)
                        | PORTMUX_USART3_ALT1_gc;    // USART3 na PF4/PF5
}*/

void portmux_init(void) { // Tested on "Nano Every"
    PORTMUX.USARTROUTEA = 
                      PORTMUX_USART0_DEFAULT_gc  // USART0: PA1 (Rx), PA0 (Tx)
                    | PORTMUX_USART1_ALT1_gc     // USART1: PC5 (Rx), PC4 (Tx)
                    | PORTMUX_USART2_ALT1_gc     // USART2: PF5 (Rx), PF4 (Tx)
                    | PORTMUX_USART3_DEFAULT_gc; // USART3: PB1 (Rx), PB0 (Tx) ?USB/CDC?
}

uint16_t calc_baud(uint32_t baud_rate, uint32_t f_cpu) { // UART baud rate
    return (f_cpu / (2 * baud_rate)) - 1;
}

void uart0_init(uint32_t baud_rate) {
    uint16_t baud = calc_baud(baud_rate, F_CPU); // baud rate 
    USART0.BAUD = baud;
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; 
    USART0.CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm; 
    PORTA.DIRSET = PIN0_bm;     // PA0 TX za USART0
    PORTA.DIRCLR = PIN1_bm;     // PA1 Rx za USART0
}

void uart1_init(uint32_t baud_rate) {
    uint16_t baud = calc_baud(baud_rate, F_CPU); // baud rate 
    USART1.BAUD = baud;
    USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm; 
    USART1.CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm; 
    PORTC.DIRSET = PIN4_bm;     // TX PC4 za USART1
    PORTC.DIRCLR = PIN5_bm;     // RX PC5 za USART1
}

void uart2_init(uint32_t baud_rate) {
    uint16_t baud = calc_baud(baud_rate, F_CPU); // baud rate 
    USART2.BAUD = baud;
    USART2.CTRLB = USART_RXEN_bm | USART_TXEN_bm; 
    USART2.CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm; 
    PORTF.DIRSET = PIN4_bm;     // TX PF4 za USART2
    PORTF.DIRCLR = PIN5_bm;     // RX PF5 za USART2
}

void uart3_init(uint32_t baud_rate) {   // USB
    uint16_t baud = calc_baud(baud_rate, F_CPU); // baud rate 
    USART3.BAUD = baud;  
    USART3.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Rx in Tx
    USART3.CTRLC = USART_CHSIZE0_bm | USART_CHSIZE1_bm; // 8-bits 
    _delay_ms(50);          // Test connection TX --> PB0.
    PORTB.DIRSET = PIN0_bm; // PB0 as USART3.TX 
    PORTB.DIRCLR = PIN1_bm; // PB1 as USART3.RX
}

void uart0_send(char data) {
    while (!(USART0.STATUS & USART_DREIF_bm)); 
    USART0.TXDATAL = data; 
}

void uart1_send(char data) {
    while (!(USART1.STATUS & USART_DREIF_bm)); 
    USART1.TXDATAL = data; 
}

void uart2_send(char data) {
    while (!(USART2.STATUS & USART_DREIF_bm)); 
    USART2.TXDATAL = data; 
}

void uart3_send(char data) {
    while (!(USART3.STATUS & USART_DREIF_bm));
    USART3.TXDATAL = data; 
}

char uart0_receive(void) { 
    while (!(USART0.STATUS & USART_RXCIF_bm)); 
    return USART0.RXDATAL;  
}
char uart0_receive_nonblocking(void) {      // Non-blocking receiving
    if (USART0.STATUS & USART_RXCIF_bm) {   // New data?
        return USART0.RXDATAL;              // Return
    }
    return -1; // No data,... -1
}

char uart1_receive(void) {
    while (!(USART1.STATUS & USART_RXCIF_bm));
    return USART1.RXDATAL;  
}
char uart1_receive_nonblocking(void) {
    if (USART1.STATUS & USART_RXCIF_bm);
        return USART1.RXDATAL;  
    return -1;        
}

char uart2_receive(void) {
    while (!(USART2.STATUS & USART_RXCIF_bm));
    return USART2.RXDATAL;  
}
char uart2_receive_nonblocking(void) {
    if (USART2.STATUS & USART_RXCIF_bm);
        return USART2.RXDATAL;
    return -1;  
}

char uart3_receive(void) {
    while (!(USART3.STATUS & USART_RXCIF_bm));
    return USART3.RXDATAL;  
}
char uart3_receive_nonblocking(void) {
    if (USART3.STATUS & USART_RXCIF_bm);
        return USART3.RXDATAL;  
    return -1;
}