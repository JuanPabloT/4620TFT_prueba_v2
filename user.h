/* 
 * File:   user.h
 * Author: Matias Ferraro
 *
 * Created on 16 de agosto de 2018, 10:32
 */

#ifndef USER_H
#define	USER_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

    /******************************************************************************/
    /* User Level #define Macros                                                  */
    /******************************************************************************/



#define	BAUDRATE    9600

#define PIN_1    PORTCbits.RC2       // Defino
#define TRIS_1   TRISCbits.TRISC2    //  

#define PIN_2   PORTDbits.RD5       // Defino
#define TRIS_2   TRISDbits.TRISD5    //  
    
#define PIN_3   PORTDbits.RD6       // Defino
#define TRIS_3   TRISDbits.TRISD6    //  
    
#define PIN_4   PORTDbits.RD7       // Defino
#define TRIS_4   TRISDbits.TRISD7    //  
    /******************************************************************************/
    /*Tipos de datos declarados por el usuario                                    */
    /******************************************************************************/

    typedef uint32_t tick_t;    // Define tipo de dato para Tick
    extern volatile tick_t tickCounter; // Contador de Tick
    extern volatile uint8_t pwm_manual,pwm_aux; 
    // Nuevo tipo de datos enumerado llamado estadoMEF



    /******************************************************************************/
    /* User Function Prototypes                                                   */
    /******************************************************************************/

void InitApp(void); // I/O and Peripheral Initialization
tick_t tickRead(void); // Lee Tick Counter
void tickWrite(tick_t ticks);
void uartConfig(void);  /* Inicialización de UART */
void putch(char data);  /* pone un byte e stdout */
char getch(void);       /* toma un byte e stdin  */
char getche(void);      /* toma un byte e stdin con eco */
void uartWriteByte( uint8_t value );        /* Envia un byte en forma boqueante */
uint8_t uartReadByte( uint8_t* receivedByte );  /* Toma un byte de la UART */
void myDelayms(uint16_t cant_ms);/* demora en mseg por interrupciones*/

void inimef(void);
void inimefcora(void);
void actmef(void);
void actmefcora(void);




#ifdef	__cplusplus
}
#endif

#endif	/* USER_H */