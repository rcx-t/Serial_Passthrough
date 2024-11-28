/**
 * @file SerialPassthrough.cpp
 * @brief Link serial trafic between Seria0 / Serial1 (and Serial2 / Serial3)
 * @author jzupan
 * @date 2023/24
 * @note Tested on Arduino NANO Every board ( Atmega4809 CPU )
 * @note This testing code:
 * ```
 * Arduino library:
 * -  Sending bytes on Serial, and Serial1 every 300ms    or
 * -  Receiving bytes from Serial and transmitting to Serial1 and vice versa.
 * AVR libraries (without any use of ISRs):
 * -  Sending bytes on UART0, UART1, UART2, UART3 every 500ms    or
 * -  Receiving bytes from UART0 and transmitting to USART1 and vice versa and
 *    Receiving bytes from UART2 and transmitting to USART3 and vice versa.
 * ```
 * @note Atmega4809 CPU 20MHz, (28,32,40,48 pin):
 * ```
 * Flash size 48kb, SRAM 6kb, EEPROM 256b, 4x 16-bit Timers, RTC, 4x USART, 1X WDT
 * 1x TWI 100/400/1000 kHz, 1x(16) 10-bit AD lines, 1x Custom Logic (LUT), ...
 * ```
 */
// Choose:
#define AVR_LIBS  1     // AVR libraries: 4 channels (no USB/CDC)
#define ARDU_LIBS 2     // Arduino libraries: 2 channels (including USB/CDC)
#define LIBS AVR_LIBS 

#define TXmode    3     // Transmit bytest to Serial=USART3 & Serial1=USART1 or UART0,1,2,3
#define TXRXmode  4     // Transmit/Receive between Serial & Serial1 or UART0-UART2, UART1-UART2
#define Testmode  RXTXmode

/**
 * @image   html "nano_every.png"  "Arduino NANO EVERY modul"  height=200px
 */

#if LIBS == ARDU_LIBS
  #include <Arduino.h>
  #include <util/delay.h>
#else
  #define F_CPU 2566666L
  #include <avr/io.h>
  #include <util/delay.h>
  #include "Serial.h"
#endif

/* MAIN
 []------------------------------------------------------------------------[]
*/
#if LIBS == ARDU_LIBS

/* 
 []------------------------------------------------------------------------[]
*/
void led_toggle (void) {
static uint8_t ff = 0;
  ff = ff ? 0 : 1;
  if (ff) digitalWrite(LED_BUILTIN, HIGH);
  else    digitalWrite(LED_BUILTIN, LOW);
}

/* Arduino, 2 channels, ...
 []------------------------------------------------------------------------[]
*/
void setup() {
uint8_t n = 60;
  pinMode(LED_BUILTIN, OUTPUT); // pin D13 (PE2)

  while(n--) { 
    delay(30);
    led_toggle(); 
  }

  Serial.begin(115200);  // USB/CDC
  Serial1.begin(115200); // USART1  TTL Rx=pin0 (PC5), Tx=pin1 (PC4)
}

# if Testmode == TXmode
/* Arduino: Transmit bytes on Serial, Serial1
 []------------------------------------------------------------------------[]
*/
void loop() {

    Serial.write('0');  // USB/CDC == USART3
    _delay_us(120);

    Serial1.write('1'); // USART1 TTL Rx=pin D0 (PC5), Tx=pin D1 (PC4)
    _delay_us(120);     

    delay(300);         // ms
    led_toggle();

}

# else  // Arduino: Testmode == RXTXmode

void loop() {

  if (Serial.available())
  {                                   // If anything comes in Serial (USB), ( 0 -- 1)
    Serial1.write(n = Serial.read()); // read it and send it out Serial1 (pins 0 & 1)
    if (n == '\n')
      digitalWrite(LED_BUILTIN, HIGH);
  }

  if (Serial1.available())
  {                                   // If anything comes in Serial1 (pins 0 & 1)  ( 1 --> 0)
    Serial.write(n = Serial1.read()); // read it and send it out Serial (USB)
    if (n == '\n')
      digitalWrite(LED_BUILTIN, LOW);
  }

}
#endif

#else // AVR ... (No interrrupts & buffers!)

/* 
 []------------------------------------------------------------------------[]
*/
void led_toggle(void) {
    PORTE.OUTTGL = PIN2_bm; // Toggle LED na PE2 
}

/* 
 []------------------------------------------------------------------------[]
*/
int main(void) {
char n = 50;

  PORTE.DIRSET = PIN2_bm;   // LED
  while(n--) { 
    _delay_ms(30);
    led_toggle(); 
  }
    portmux_init();
    uart0_init(9600);
    uart1_init(9600);
    uart2_init(9600);
    uart3_init(9600);

#if Testmode == TXmode  // AVR: TX only

  while(1) { // main loop - TRANSMITTING ONLY
    uart0_send('0');

    uart1_send('1');
    _delay_us(200);
    uart1_send('1');
    _delay_us(200);

    uart2_send('2');
    _delay_us(200);
    uart2_send('2');
    _delay_us(200);
    uart2_send('2');
    _delay_us(200);

    uart3_send('3');
    _delay_us(200);
    uart3_send('3');
    _delay_us(200);
    uart3_send('3');
    _delay_us(200);
    uart3_send('3');

    _delay_ms(500);
    led_toggle();
  }

#else   // Testmode == RXTXmode  AVR Rx & Tx

  while (1) {
      n = uart0_receive_nonblocking(); // New USART0 data ?
      if (n != -1) { 
          uart1_send(n);   // Send it on USART1
          if (n == '\n') { // And toggle LED if EOL
              led_toggle();
          }
      }

      n = uart2_receive_nonblocking(); // New USART2 data ?
      if (n != -1) { 
          uart3_send(n);   // Send it on USART3
          if (n == '\n') { // And toggle LED if EOL
              led_toggle();
          }
      }
  }

#endif

}
#endif


// Doddatna dokumentacija (SI):
/**
 * @mainpage Testiranje oddaje in prometa med serijskimi porti oziroma UART-i <img src="SIflag.gif" alt="SI" style="float: right; height: 25px;">
 * 
 * @section uvod Uvod
 * 
 * Preprost program za asinhrono oddajanje in medsebojno povezavo UART parov na procesorju Atmega4809.
 * 
 * @image   html "NANO_Every_jz.png"  "Arduino NANO EVERY modul"  height=350px
 * 
 * @section sekcija1 Dodatno 
 * 
 * Uporabljamo Arduino ali AVR set knjiznic.<br><br>
 * <b>Arduino</b> temelji na podpori objektov Serial in Serial1.<br>
 * <b>AVR</b> pa na podpori USART0 1, 2 in 3.<br>
 * Oddajanje ali navzkrizni sprejem/oddajo izberem v prvih vrsticah te kode.<br>
 * UART0 <---> UART1 @115.200 baud.<br>
 * UART2 <---> UART3 @115.200 baud.<br><br>
 * Program je napisan za Platformio IDE.
 */
