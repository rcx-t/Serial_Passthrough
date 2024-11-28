/**
 * @file Serial.h
 * @brief (Header for 4/2/1 channel Serial driver)
 * @author jzupan 
 * @date 2023/24
 */

#ifndef SERIAL_H_
#define SERIAL_H_

// Proc auto detection
// according to the instructions of Ardhat / ArdhatMFC 2013/4
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define PROMINI
#endif

#if defined(__AVR_ATmega32U4__) 
  #define PROMICRO
#endif

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || \
    defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__) || \
    defined(__AVR_ATmega4809__)
  #define MEGA
#endif

#if defined MEGA  // Only supported, for now.

void portmux_init(void);
void portmux_usart3_to_pb0(void);

void uart0_init(uint32_t);
void uart0_send(char data);
char uart0_receive(void);
char uart0_receive_nonblocking(void);

void uart1_init(uint32_t);
void uart1_send(char data);
char uart1_receive(void);
char uart1_receive_nonblocking(void);

void uart2_init(uint32_t);
void uart2_send(char data);
char uart2_receive(void);
char uart2_receive_nonblocking(void);

void uart3_init(uint32_t);
void uart3_send(char data);
char uart3_receive(void);
char uart3_receive_nonblocking(void);

#endif

#endif /* SERIAL_H_ */