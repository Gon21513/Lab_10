/* 
 * File:   prelab10.c
 * Author: lpgp3
 *
 * Created on 30 de abril de 2023, 01:47 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR enabled)
#pragma config IESO = OFF        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)



#include<xc.h>
#include<pic.h>
#include<stdint.h>

//-------------------------------variables------------------------------------|
uint8_t counter;

//|------------------------------prototipos------------------------------------|

void setup(void);
void __interrupt() isr(void);


void    main(void){
    setup();
    while(1){
        PORTA   =   counter; //poner el valor del contador en el puerto A
        
    }
}

//|------------------------------Setup----------------------

void setup(void){
    
//-------------configuracion de puertos----------------
    ANSEL   =   0;
    ANSELH  =   0;
    TRISA   =   0;
    
     //botones
    TRISBbits.TRISB0 = 1; //rb0 como entrada
    TRISBbits.TRISB1 = 1; //rb1 como entrada 
    
//----------pullups------------------
    OPTION_REGbits.nRBPU = 0; //habilitarr pullups
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1; 
    
    IOCBbits.IOCB0 = 1; //habilitar interrupciones en rb0
    IOCBbits.IOCB1 = 1; // habilitar interrupciones en rb1

//------------interrupciones-----------------
    INTCONbits.GIE = 1; //habilitar interrupciones globales
    INTCONbits.RBIE = 1; //habilitar interrupciones en portb
    INTCONbits.RBIF = 0; //limpirar bander de interrupcion de portb
    
    
   //Se inician los puertos 
    PORTA   =   0;
    PORTB   =   0;

    
    //Se inicia el contador en 0
    counter =   0;
}

       


//----------------Interrupcion--------------------
void __interrupt() isr(void) {
    // Comprobar si se ha producido una interrupción en PORTB 
    if (RBIF) {
        // Si RB0 está presionado  y RB1 no está presionado 
        if (RB0 == 0 && RB1 == 1) {
            counter++; // Incrementar el contador
        }
        // Si RB0 no está presionado  y RB1 está presionado 
        if (RB0 == 1 && RB1 == 0) {
            counter--; // Decrementar el contador
        }
        // Limpiar la bandera de interrupción de PORTB 
        RBIF = 0;
    }
}