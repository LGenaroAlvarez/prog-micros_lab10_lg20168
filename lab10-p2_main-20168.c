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
#include <stdio.h>
#include <stdint.h>

//DEFINICION DE FRECUENCIA PARA DELAY
#define _XTAL_FREQ 1000000          // FRECUENCIA PARA DELAYS (1MHz)

//VARIABLES GLOBALES
int state_flag = 0;                 // BANDERA PARA ESTADO EN EDICION
uint8_t pot1;                       // VARIABLE PARA LEER POTENCIOMETRO
uint8_t opt_sel;                    // VARIABLE PARA ALMACENAR OPCION SELECCIONADA
uint8_t portb_char;                 // VARIABLE PARA ALMACENAR VALOR DADO POR INTERFACE
unsigned char vacio = '\r';         // VARIABLE CON CARACTER DE ENTER PARA FORMATEAR TEXTO

//PROTO FUNCIONES
void setup(void);                   // FUNCION DE SETUP
void item_list(void);               // FUNCION PARA MENU PRINCIPAL
void enter(int a);                  // FUNCION PARA COLOCAR UNA CANTIDAD ESTABLECIDA DE ENTERS

//CONFIGURACION PRINCIPAL
void setup(void){
    ANSEL = 0b00000001;             // AN0 COMO ANALOGICO, EL RESTO COMO DIGITAL
    ANSELH = 0;                     // I/O DIGITALES

    TRISA = 0b00000001;             // PIN0 PORTA COMO ENTRADA
    TRISB = 0;                      // PORTB COMO SALIDA
    PORTA = 0;                      // LIMPIEZA DE PORTA
    PORTB = 0;                      // LIMPIEZA DE PORTB
    
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
    TXSTAbits.SYNC = 0;             // MODO ASINCRONO
    TXSTAbits.BRGH = 1;             // 
    BAUDCTLbits.BRG16 = 1;          // 
    SPBRGH = 0;                     // BYTE SUPERIOR
    SPBRG = 25;                     // BAUD RATE APROXIMADO (9600)
    RCSTAbits.SPEN = 1;             // ACTIVAR EUSART CON TX/CK COMO SALIDA
    TXSTAbits.TXEN = 1;             // ACTIVADO DE TRANSMISOR DEL EUSART
    RCSTAbits.CREN = 1;             // ACTIVADO DEL RECEPTOR DEL EUSART
    PIE1bits.RCIE = 1;              // ACTIVAR INTERRUPCIONES EN RECEPTOR
}

//FUNCION PARA ENTERS DE CANTIDAD ESPECIFICADA
void enter(int a){
    while(a>0){                     // MIENTRAS EL VALOR INDICADO SEA MAYOR A CERO
        a--;                        // DISMINUIR EL VALOR
        __delay_ms(40);             // DELAY PARA EJECUCION
        TXREG = vacio;              // CARGAR UN ENTER A LA CONSOLA
    }
}

//FUNCION DE IMPRESION
void tprint(char *string){          // CONVERTIR STRIGN A PUNTERO
    while(*string != '\0'){         // REVISAR QUE EL VALOR DEL PUNTERO NO SEA NULO
        while(TXIF != 1);           // SI NO ES NULO REVISAR QUE LA BANDERA DE INTERRUPCION ESTE ACTIVADA
        TXREG = *string;            // SI ESTA ACTIVADA CARGAR CARACTER DEL PUNTERO A LA CONSOLA
        *string++;                  // INCREMENTAR DE POSICION EN EL PUNTERO
    }
}

//INTERRUPCIONES
void __interrupt() isr(void){
    if (PIR1bits.ADIF){             // REVISAR SI HAY INTERRUPCION DE ADC
        if (ADCON0bits.CHS == 0){   // REVISAR SI HAY INTERRUPCION EN EL CANAL 0
           pot1 = ADRESH;           // CARGAR VALOR DEL ADRESH A VARIABLE DE POTENCIOMETRO
        }
        PIR1bits.ADIF = 0;          // LIMPIAR BANDERA DE INTERRUPCION DE ADC
    }
    
    if (RCIF){                      // REVISAR SI HAY INTERRUPCION DE RECEPTOR
        if (state_flag == 0){       // REVISAR SI SE ENCUENTRA EN EL ESTADO 0
            opt_sel = RCREG;        // SI SE ENCUENTRA EN EL ESTADO 0 CARGAR CARACTER A VARIABLE DE SELECCION DE OPCION
            RCREG = 0;              // LIMPIAR REGISTRO DEL RECEPTOR
        }
        else if (state_flag == 1){  // REVISAR SI SE ENCUENTRA EN EL ESTADO 1
            state_flag = 0;         // LIMPIAR BANDERA DE ESTADO
            portb_char = RCREG;     // CARGAR VALOR DEL RECEPTOR A VARIABLE DE CARACTER INTERNA
            TXREG = portb_char;     // MOSTRAR CARACTER PRESIONADO EN LA CONSOLA
            PORTB = portb_char;     // MOSTRAR CARACTER PRESIONADO EN EL PORTB EN ASCII
            RCREG = 0;              // LIMPIAR REGISTRO DEL RECEPTOR
            enter(1);               // IMPRIMIR UN ENTER
            item_list();            // IMPRIMIR MENU EN HIPERCONSOLA
        }               
    }
    return;
}

void main(void) {
    //EJECUCION CONFIG
    setup();
    item_list();                    // IMPRIMIR MENU EN HIPERCONSOLA
    
    //LOOP MAIN
    while(1){
        if (ADCON0bits.GO == 0){    // REVISAR SI SE HA INICIADO LA CONVERSION DEL ADC
            ADCON0bits.GO = 1;      // INICIAR PROCESO DE CONVERSION
        }
        if (opt_sel == 0x61){       // SI SE HA SELECIONADO LA OPCION (a)
            opt_sel = 0x00;         // LIMPIAR REGISTRO DE SELECION
            
            tprint("Valor del potenciometro: "); // IMPRIMIR MENSAJE DE VALOR A MOSTRAR
            __delay_ms(500);        // TIEMPO DE ESPERA PARA ADQUIRIR DATOS
            TXREG = pot1;           // CARGAR VALOR DEL POTENCIOMETRO A LA HIPERCONSOLA EN FORMATO ASCII
            __delay_ms(500);        // TIEMPO DE ESPERA
            item_list();            // IMPRIMIR MENU EN HIPERCONSOLA
        }
        else if (opt_sel == 0x62){  // SI SE HA SELECIONADO LA OPCION (b)
            state_flag = 1;         // SETEAR BANDERA DE ESTADO 1
            tprint("Ingrese caracter en ASCII: ");  // IMPRIMIR MENSAJE SOLICITANDO UN VALOR
            opt_sel = 0x00;         // LIMPIAR REGISTRO DE SELECCION
        }
    }
    return;
}

// MENU EN HIPERTERMINAL
void item_list(){
    enter(2);
    tprint("----------------Main Menu-------------------");
    enter(1);
    tprint("a) Lectura del potenciometro.");
    enter(1);
    tprint("b) Enviar ASCII.");
    enter(2);
}