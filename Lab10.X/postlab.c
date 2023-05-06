/* 
 * File:   postlab.c
 * Author: lpgp3
 *
 * Created on 3 de mayo de 2023, 07:11 PM
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ  4000000
#include<xc.h>
#include<pic.h>
#include<stdint.h>
#include<string.h>

//|----------------------------------------------------------------------------|
//|-------------------------------VARIABLES------------------------------------|
//|----------------------------------------------------------------------------|
uint8_t counter;
uint8_t counter_comp;


//|----------------------------------------------------------------------------|
//|------------------------------PROTOTYPES------------------------------------|
//|----------------------------------------------------------------------------|
void setup(void);
void __interrupt() isr(void);

void UART_Write(unsigned char* word);
void UART_Write_Char(uint8_t character);
//|----------------------------------------------------------------------------|
//|---------------------------------CODE---------------------------------------|
//|----------------------------------------------------------------------------|

void    main(void){
    setup();
    while(1){
        PORTA   =   counter;
        
        
        if(counter_comp  != counter){
            UART_Write_Char(counter);
            counter_comp    =   counter;
        }
        
        
        if(RCIF){
            PORTD   =   RCREG;
        }
    }
}
//|----------------------------------------------------------------------------|
//|------------------------------FUNCTIONS-------------------------------------|
//|----------------------------------------------------------------------------|

void setup(void){
//-------------configuracion de puertos----------------
    ANSEL   =   0;
    ANSELH  =   0;
    TRISA   =   0;
    TRISD = 0;
    
     //botones
    TRISBbits.TRISB0 = 1; //rb0 como entrada
    TRISBbits.TRISB1 = 1; //rb1 como entrada 
    
    TRISCbits.TRISC7 = 1; //rb0 como entrada 
    
//----------pullups------------------
    OPTION_REGbits.nRBPU = 0; //habilitarr pullups
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1; 
    
    IOCBbits.IOCB0 = 1; //habilitar interrupciones en rb0
    IOCBbits.IOCB1 = 1; // habilitar interrupciones en rb1

//------------interrupciones-----------------
    INTCONbits.GIE = 1; //habilitar interrupciones globales
    INTCONbits.RBIE = 1; //habilitar interrupciones en portb
    INTCONbits.PEIE = 1;
    INTCONbits.RBIF = 0; //limpirar bander de interrupcion de portb
    
    
   //Port Inicialization
    PORTA   =   0;
    PORTB   =   0;
    PORTD   =   0;
    
//----------UART---------------------------------
    TXSTAbits.SYNC = 0;//asincrono
    TXSTAbits.BRGH = 1;//high baud rate select bit
    
    BAUDCTLbits.BRG16 = 0;//utilizar 16 bits baud rate
    
    SPBRG = 25; //configurar a 9615
    SPBRGH = 0;    

    
    RCSTAbits.SPEN = 1;//habilitar la comunicacion serial
    RCSTAbits.RX9 = 0;//deshabiliamos bit de direccion
    RCSTAbits.CREN = 1;//habilitar recepcion 
    TXSTAbits.TXEN = 1;//habiliar la transmision
    
    //Variable Inicialization
    counter =   0;
    counter_comp    =   255;
}

       

//|----------------------------------------------------------------------------|
//|------------------------------INTERRUPTS------------------------------------|
//|----------------------------------------------------------------------------|
void __interrupt() isr(void){
    if(RBIF){
        if(RB0==0 && RB1==1){
            counter++;
        }
        if(RB0==1 && RB1==0){
            counter--;
        }
        RBIF    =   0;
        
    }

}



    
void UART_Write(unsigned char* word){   
    while (*word != 0){                 //Loop until NULL
        TXREG = (*word);                //Send current array value pointed
        while(!TXSTAbits.TRMT);         //Make sure TSR is full (value sent)
        word++;                         //Go to next value in the array
    }
}

void UART_Write_Char(uint8_t character){
    TXREG   =   character;
    while   (!TXSTAbits.TRMT);
}