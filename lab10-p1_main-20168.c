/*
 * File:   lab10_main-20168.c
 * Author: luisg
 *
 * Created on May 4, 2022, 12:00 PM
 */
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include<stdio.h>
#include <stdint.h>

//DEFINICION DE FRECUENCIA PARA DELAY
#define _XTAL_FREQ 1000000          // FRECUENCIA PARA DELAYS (1MHz)

//PROTO FUNCIONES
void setup(void);                   // FUNCION DE SETUP

//INTERRUPCIONES
void __interrupt() isr(void){
    return;
}

//CONFIGURACION PRINCIPAL
void setup(void){
    ANSEL = 0;                      //
    ANSELH = 0;                     //
    
    TRISB = 0;                      //
    PORTB = 0;                      //
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0100;       // FRECUENCIA DE OSCILADOR INTERNO (1MHz)
    OSCCONbits.SCS  = 1;            // RELOJ INTERNO
    
    //SERIAL CONFIG
    TXSTAbits.SYNC = 0;             //
    TXSTAbits.BRGH = 1;             //
    BAUDCTLbits.BRG16 = 1;          //
    SPBRGH = 0;                     //
    SPBRG = 25;                     //
    RCSTAbits.SPEN = 1;             //
    RCSTAbits.RX9 = 0;              //
    TXSTAbits.TXEN = 1;             //
    RCSTAbits.CREN = 1;             //
    INTCONbits.GIE = 1;             //
    INTCONbits.PEIE = 1;            //
    PIE1bits.RCIE = 1;              //
}

void main(void) {
    //EJECUCION CONFIG
    setup();
    
    while(1){
        __delay_ms(1000);           //
        if (PIR1bits.TXIF){         //
                TXREG = 0x41;       //
        }
    }
    return;
}
