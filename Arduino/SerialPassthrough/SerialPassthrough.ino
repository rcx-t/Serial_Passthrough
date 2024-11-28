/**
 * @file SerialPassthrough.ino
 * @brief Link serial trafic between Serial <--> Serial1 AND Serial2 <--> Serial3
 * @author jzupan
 * @date 2023
 * @note Arduino IDE & libraries are used here
 * @note With "cross_port_or_asy_Tx", we can select:
 * - cyclic transmitting bytes to all Serial() ports (0,1,2,3) or 
 * - receive/transmit bytes between two cross-connected Serial port pairs
 * @note Use a copy of patched "pins_arduino.h" originally placed in:
 * @nore C:/Users/user/AppData/Local/arduino15/packages/arduino/hardware/megaavr/1.8.5/variants/nona4809/pins_arduino.h
 * @note Patches enables Serial2 & 3
 * @note Atmega4809 CPU USARTs:
 * ```
 * Serial  == USB/CDC (USART3)
 * Serial1 == USART1
 * Serial2 == USART0
 * Serial3 == USART2
 * ```
 */
#include <Arduino.h>
#include <util/delay.h>

#define cross_port 0
#define async_tx  1
#define cross_port_or_asy_Tx 1    // <========== select here

/**
 * @image   html "nano_every.png"  "Arduino NANO EVERY modul"  height=150px
 */

 /*
 []------------------------------------------------------------------------[]
*/
void led_toggle (void) {
static uint8_t ff = 0;
  ff = ff ? 0 : 1;
  if (ff) digitalWrite(LED_BUILTIN, HIGH);
  else    digitalWrite(LED_BUILTIN, LOW);
}

/*
 []------------------------------------------------------------------------[]
*/
void setup() {
uint8_t n = 60;
  pinMode(LED_BUILTIN, OUTPUT); // pin D13 (PE2)

  while(n--) { // Intro
    delay(30);
    led_toggle(); 
  }

  Serial.begin(115200);  // (USART3)  USB/CDC
  Serial1.begin(115200); //  USART1  TTL Rx=pin0 (PC5), Tx=pin1 (PC4)
  Serial2.begin(115200); //  USART0  TTL Rx=pin7 (PA1), Tx=pin2 (PA0)
  Serial3.begin(115200); //  USART2  TTL Rx=pin6 (PF4), Tx=pin3 (PF5) 
}

char n = 0;

/* Transmit bytes on USART0, 1, 2, & 3
 []------------------------------------------------------------------------[]
*/
void loop() {

#if cross_port_or_asy_Tx == async_tx

  while(1) // Sending to all Serial ports
  {
    Serial.write('0');  // USB/CDC == USART3
    _delay_us(120);

    Serial1.write('1'); // TTL Rx=pin D0 (PC5), Tx=pin D1 (PC4)
    _delay_us(120);     

    Serial2.write('2'); // TTL Rx=pin D7 (PA1), Tx=pin D2 (PA0)
    _delay_us(120);
    Serial2.write('2');
    _delay_us(120);

    Serial3.write('3'); // TTL Rx=pin D6 (PF4), Tx=pin D3 (PF5) 
    _delay_us(120);
    Serial3.write('3');
    _delay_us(120);
    Serial3.write('3');

    delay(300); // ms
    led_toggle();
  }

#else 

/*  or Cross connect Serial <==> Serial1 & Serial2 <==> Serial3
 []------------------------------------------------------------------------[]
*/
  if (Serial.available()) {            // If anything comes in Serial (USB),
    Serial1.write(n = Serial.read());  // read it and send it out Serial1 (pins 0 & 1)
    if (n == '\n') digitalWrite(LED_BUILTIN, HIGH);
  }

  if (Serial1.available()) {            //  
    Serial.write(n = Serial1.read());   //  
    if (n == '\n') digitalWrite(LED_BUILTIN, LOW);
  }

  if (Serial2.available()) {            //  
    Serial3.write(n = Serial2.read());  //  
    if (n == '\n') digitalWrite(LED_BUILTIN, HIGH);
  }

  if (Serial3.available()) {            //  
    Serial2.write(n = Serial3.read());  //  
    if (n == '\n') digitalWrite(LED_BUILTIN, LOW);
  }
#endif

}
 
// Še doddatna dokumentacija (SI): 
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
 * Serial  <---> Serial1 @115.200 baud.<br>
 * Serial2 <---> Serial3 @115.200 baud.<br>
 * Pomni!<br>
 * Serial2 in 3 delujeta samo po spremembi sicer priloženega "pins_arduino.h"!<br><br>
 * Program je napisan za Arduino IDE in deluje.
 */


