/*
 * File:   lab10-p2_main-20168.c
 * Author: luisg
 *
 * Created on May 4, 2022, 2:24 PM
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

//VARIABLES GLOBALES
int state_flag = 0;                 // BANDERA PARA DISPLAYS 
uint8_t pot1;                       // VARIABLE PARA LEER POTENCIOMETRO
uint8_t opt_sel;                    // VARIABLE PARA ALMACENAR OPCION SELECCIONADA
uint8_t portb_char;
unsigned char vacio = '\r';

//PROTO FUNCIONES
void setup(void);                   // FUNCION DE SETUP
void item_list(void);               // 
void enter(int a);                  // 

//CONFIGURACION PRINCIPAL
void setup(void){
    ANSEL = 0b00000001;             // 
    ANSELH = 0;                     // 

    TRISA = 0b00000001;             //
    TRISB = 0;                      // 
    PORTA = 0;                      // 
    PORTB = 0;                      // 
    
    //CONFIG DE INTERRUPCIONES
    INTCONbits.GIE = 1;             // HABILITAR INTERRUPCIONES GLOBALES
    INTCONbits.PEIE = 1;            // HABILITAR INTERRUPCIONES EN PERIFERICOS
    PIR1bits.ADIF = 0;              // LIMPIEZA DE BANDERA DE INTERRUPCION DE ADC
    PIE1bits.ADIE = 1;              // HABILITAR INTERRUPCION DE ADC
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0100;       // FRECUENCIA DE OSCILADOR INTERNO (1MHz)
    OSCCONbits.SCS  = 1;            // RELOJ INTERNO
    
    //ADC CONFIG
    ADCON0bits.ADCS = 0b01;         // FOSC/8
    ADCON1bits.VCFG0 = 0;           // USO DE VDD COMO VOLTAJE DE REFERENCIA INTERNO
    ADCON1bits.VCFG1 = 0;           // USO DE VSS COMO VOLTAJE DE REFERENCIA INTERNO

    ADCON0bits.CHS = 0b0000;        // SELECCION DE PORTA PIN0 (AN0) COMO ENTRADA DE ADC
    ADCON1bits.ADFM = 0;            // FORMATO DE BITS JUSTIFICADOS A LA IZQUIERDA
    ADCON0bits.ADON = 1;            // HABILITACION DE MODULO DE ADC
    __delay_us(40);                 // TIEMPO DE LECTURA
    
    //SERIAL CONFIG 
    TXSTAbits.SYNC = 0;             // 
    TXSTAbits.BRGH = 1;             // 
    BAUDCTLbits.BRG16 = 1;          // 
    SPBRGH = 0;                     // 
    SPBRG = 25;                     // BAUD RATE APROXIMADO (9600)
    RCSTAbits.SPEN = 1;             // 
//    RCSTAbits.RX9 = 0;              // 
    TXSTAbits.TXEN = 1;             // 
    RCSTAbits.CREN = 1;             // 
}

void enter(int a){
    while(a>0){
        a--;
        __delay_ms(40);
        TXREG = vacio;
    }
}

//FUNCION DE IMPRESION
void tprint(char *string){ // 
    while(*string != '\0'){         // 
        while(TXIF != 1);           // 
        TXREG = *string;            // 
        *string++;                  // 
    }
    enter(1);
}

//INTERRUPCIONES
void __interrupt() isr(void){
    if (PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0){
           pot1 = ADRESH; 
        }
        PIR1bits.ADIF = 0;
    }
    
    if (RCIF){
        if (state_flag == 0){
            opt_sel = RCREG;
            RCREG = 0;
        }
        else if (state_flag == 1){
            state_flag = 0;
            portb_char = RCREG;
            TXREG = portb_char;
            RCREG = 0;
            enter(1);
            item_list();
        }
    }
    return;
}

void main(void) {
    //EJECUCION CONFIG
    setup();
    item_list();
    
    while(1){
        if (ADCON0bits.GO == 0){
            ADCON0bits.GO = 1;
        }
        if (opt_sel == 0x61){
            opt_sel = 0x00;
            
            tprint("Valor del potenciometro: ");
            __delay_ms(500);
            TXREG = pot1;
            __delay_ms(500);
            item_list();
        }
        else if (opt_sel == 0x62){
            state_flag = 1;
            tprint("Ingrese caracter en ASCII: ");
            opt_sel = 0x00;
        }
    }
    return;
}

// MENU EN HIPERTERMINAL
void item_list(){
    enter(2);
    tprint("----------------Main Menu-------------------");
    tprint("a) Lectura del potenciometro.");
    tprint("b) Enviar ASCII.");
    enter(2);
}   