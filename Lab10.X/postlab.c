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

//-----------------------------variables------------------------------------|
uint8_t counter; // Variable contador, utilizada para llevar cuenta de eventos o ciclos
uint8_t counter_comp; // Variable de comparación de contador, usada para rastrear cambios en el contador


//-----------------------------prototipos------------------------------------|

void setup(void); // Prototipo para la función de configuración
void __interrupt() isr(void); // Prototipo para la rutina de servicio de interrupción

void UART_Write(unsigned char* word); // Prototipo para función que escribe una cadena de caracteres a la UART
void UART_Write_Char(uint8_t character); // Prototipo para función que escribe un solo carácter a la UART


//-----------------------------main------------------------------------|


void main(void){
    setup(); // Llamada a la función de configuración
    while(1){
        PORTA = counter; // Asigna el valor del contador al puerto A
        
        
        if(counter_comp  != counter){ // Si el valor de comparación del contador es diferente al contador
            UART_Write_Char(counter); // Escribe el valor del contador a la UART
            counter_comp = counter; // Actualiza el valor de comparación del contador
        }
        
        
        if(RCIF){ // Si el flag de interrupción de recepción está establecido
            PORTD = RCREG; // Lee el registro de recepción en el puerto D
        }
    }
}


//-----------------------------Setup------------------------------------|

void setup(void){
//-------------configuracion de puertos----------------
    ANSEL   =   0;
    ANSELH  =   0;
    TRISA   =   0;
    TRISD = 0;
    
     // Configuración de botones
    TRISBbits.TRISB0 = 1; // Configura RB0 como entrada
    TRISBbits.TRISB1 = 1; // Configura RB1 como entrada
    
    TRISCbits.TRISC7 = 1; // Configura RC7 como entrada
    
//----------pullups------------------
    OPTION_REGbits.nRBPU = 0; // Habilita pullups
    WPUBbits.WPUB0 = 1;
    WPUBbits.WPUB1 = 1; 
    
    IOCBbits.IOCB0 = 1; // Habilita interrupciones en RB0
    IOCBbits.IOCB1 = 1; // Habilita interrupciones en RB1

//------------interrupciones-----------------
    INTCONbits.GIE = 1; // Habilita interrupciones globales
    INTCONbits.RBIE = 1; // Habilita interrupciones en PORTB
    INTCONbits.PEIE = 1;
    INTCONbits.RBIF = 0; // Limpia la bandera de interrupción de PORTB
    
    
   // Inicialización de puertos
    PORTA   =   0;
    PORTB   =   0;
    PORTD   =   0;
    
//----------UART---------------------------------
    TXSTAbits.SYNC = 0;// Configura UART para funcionamiento asincrónico
    TXSTAbits.BRGH = 1;// Selecciona tasa de baudios alta
    
    BAUDCTLbits.BRG16 = 0;// Utiliza 16 bits para la tasa de baudios
    
    SPBRG = 25; // Configura a 9615 baudios
    SPBRGH = 0;   

    RCSTAbits.SPEN = 1; // Habilita la comunicación serial
    RCSTAbits.RX9 = 0; // Deshabilitamos el bit de dirección
    RCSTAbits.CREN = 1; // Habilita la recepción
    TXSTAbits.TXEN = 1; // Habilita la transmisión
    
    // Inicialización de Variables
    counter = 0; // Inicializa contador
    counter_comp = 255; // Inicializa contador de comparación a 255 (Un valor que el contador nunca alcanzará en su primer ciclo)
}

//-----------------------------Interrupcion------------------------------------|

void __interrupt() isr(void){ // Función de interrupción
    if(RBIF){ // Si se activa la interrupción por cambio en PORTB
        if(RB0 == 0 && RB1 == 1){ // Si RB0 está en 0 y RB1 en 1
            counter++; // Incrementa el contador
        }
        if(RB0 == 1 && RB1 == 0){ // Si RB0 está en 1 y RB1 en 0
            counter--; // Decrementa el contador
        }
        RBIF = 0; // Limpia la bandera de interrupción de PORTB
    }
}

void UART_Write(unsigned char* word){ // Función para escribir una cadena de caracteres a la UART
    while (*word != 0){ // Loop hasta encontrar el carácter NULL (fin de la cadena de caracteres)
        TXREG = (*word); // Envia el valor actual del arreglo apuntado
        while(!TXSTAbits.TRMT); // Asegúrate de que el TSR esté lleno (valor enviado)
        word++; // Avanza al siguiente valor en el arreglo
    }
}

void UART_Write_Char(uint8_t character){ // Función para escribir un solo carácter a la UART
    TXREG = character; // Escribe el carácter al registro de transmisión
    while (!TXSTAbits.TRMT); // Espera a que el carácter se haya transmitido completamente
}