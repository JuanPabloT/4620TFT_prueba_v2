/******************************************************************************/
/* Archivos a Incluir                                                         */
/******************************************************************************/
#include <xc.h>             /* Archivo de Encabezados General XC8 */
#include "system.h"         /* Funciones/Parametros System, como osc/peripheral config */
#include "user.h"  
#include "ILI9341.h"
#include "GFX_Library.h"

volatile tick_t tickCounter; /* This global variable holds the tick count */
void disp_texto(uint8_t *cadena);

/******************************************************************************/
/* Funciones User                                                             */
/******************************************************************************/

/* <Inicializa variables de user.h e inserte codigo para algoritmos de user.> */

void InitApp(void) {
    /* TODO Inicializa Ports/PerifÃ©ricos/Interrupciones */

    ADCON1 = 0x0F; // Todos los pines digitales
    CMCON = 0X07;
    nRBPU = 0;
    TRISB = 0xFF;
    uartConfig();
    TRIS_1=0;
    TRIS_2=0;
    TRIS_3=0;
    TRIS_4=1;

    /* Inicializa perifÃ©ricos */

    // Configuro TIMER 0
    T0CS = 0; // Internal instruction cycle clock (FOSC/4)
    PSA = 0; // Habilito el PreScaler

    T0PS0=0;  //PS<2:0>: Prescaler Rate Select bits
    T0PS1=0;
    T0PS2=1;                /* 1 : 2 
                            * 1 : 4 
                             * 1 : 8
                             * 1 : 16
                             * 1 : 32<-
                             * 1 : 64
                             * 1 : 128
                             * 1 : 256 */
    
    TMR0IE = 1;

    PEIE = 1;
    GIE = 1;

}


/**
 * @brief	Configura e inicializa la UART
 * @return	Nada
 * @note    Las opciones se modifican en el código, por ejemplo la 
 * constante BAUDRATE
 * @sa      BAUDRATE    
 */
void uartConfig(void) {
#if((SYS_FREQ - BAUDRATE*16L)/(BAUDRATE*16L)) > 255 //Si necesita High Baud Rate
#if	(SYS_FREQ - BAUDRATE*64L)/(BAUDRATE*64L) > 255
#error Baud Rate demasiado Bajo
#endif
    SPBRG = (SYS_FREQ - BAUDRATE * 64L) / (BAUDRATE * 64L); //SPBRG para Low Baud Rate
    BRGH = 0;   //Selecciona Low Baud Rate
#else
    SPBRG = (SYS_FREQ - BAUDRATE * 16L) / (BAUDRATE * 16L); //SPBRG para High Baud Rate
    BRGH = 1;   //Selecciona High Baud Rate
#endif
    SYNC = 0;   //Selecciona Modo Asincronico
    SPEN = 1;   //Habilita Serial Port
    TRISC7 = 1;
    TRISC6 = 1;
    CREN = 1;   //Habilita Continuous Reception
    TXEN = 1;   //Habilita Transmision
}

/**
 * @brief	Envia un byte en la salida stdout
 * @param	data Byte o dato a transmitir
 * @return	Nada
 * @note    Define la salida estandar para la librería stdio
 */
void putch(char data) {
    while (!TXIF)
        continue;
    TXREG = data;
}

/**
 * @brief       Envia un byte en forma boqueante
 * @param[in]   value Valor a transmitir 
 * @return      Nada
 */
void uartWriteByte( uint8_t value ){
    // Wait for space in FIFO (blocking)
    while (!TXIF)
        continue;
    // Send byte
    TXREG = value;
}

/**
 * @brief	Toma un byte de la entrada stdin
 * @return	El byte recibido
 * @note    Define la entrada estandar para la librería stdio
 */
char getch(void) {
    while (!RCIF)
        continue;
    return RCREG;
}

/**
 * @brief       Toma un byte de la FIFO de recepción, chequea primero si hay un dato disponible
 * @param[out]  receivedByte Apunta al destino para el byte recibido
 * @return      1 si hay un byte recibido, 0 si no hay dato disponible 
 */
uint8_t uartReadByte( uint8_t* receivedByte ){
   if ( RCIF ){
      *receivedByte = RCREG;
      return 1;
   }
   return 0;
}

/**
 * @brief	Toma un byte de la entrada stdin con eco
 * @return  El byte recibido
 * @note
 */
char getche(void) {
    char data;
    while (!RCIF)
        continue;
    data = RCREG;
    while (!TXIF)
        continue;
    TXREG = data;
    return data;
}
// Lee Tick Counter

tick_t tickRead(void) {
    tick_t aux;
    GIE = 0; //Inicia Zona critica
    aux = tickCounter; //  Operaciones atomicas
    GIE = 1; //Finaliza la zona critica
    return aux;
}

// Escribe Tick Counter

void tickWrite(tick_t ticks) {
    GIE = 0; //Inicia Zona critica
    tickCounter = ticks; //  Operaciones atomicas
    GIE = 1; //Finaliza la zona critica
}

void myDelayms(uint16_t cant_ms){
    tick_t tinicio=tickRead();
    while((tickRead()-tinicio)<cant_ms);
}

typedef enum {estado_alto, estado_bajo } estadoCora_t;
typedef enum {estado_leyendo, estado_imprimiendo, estado_esperando,estado_parseando, estado_parseandohr } estadoDis_t;
estadoCora_t estado_Corazon ;
estadoDis_t estado_Display ;


void inimef(void){
    estado_Display = estado_leyendo;    
}
void inimefcora(void){
    estado_Corazon = estado_alto;
}

void actmef(void) {
    static uint8_t dato1, dato2, dato3, dato4;
    static char s02[10], hr[10];
    static int i;
//    display_setTextSize(2);
//    fillScreen(ILI9341_BLACK);
//    display_setCursor(0, 0);



    switch (estado_Display) {
        case estado_leyendo:

            if (uartReadByte(&dato1)) {

                if (dato1 == '*') {
                    estado_Display = estado_parseando;
                }

            }
            break;
            
        case estado_parseando:
            i=0;
            while(dato1!='#'){
                if(uartReadByte(&dato1)){
                    s02[i]=dato1;
                    i++;
                }               
            }
            s02[i - 1] = 0;
            fillScreen(ILI9341_BLACK);        //no importa lo que pongas el dislpay sigue 
            display_setCursor(0, 0);          //imprimiendo como quiere
            disp_texto(s02);

            ///////////////////////////////////////-----------------------------------------------------------
            if (dato1 == '#') {
                estado_Display = estado_parseandohr;
            }           
            break;
            
        case estado_parseandohr:
            i = 0;
            while (dato2 != '#') {
                if (uartReadByte(&dato2)) {
                    hr[i] = dato2;
                    i++;
                }
            }
            dato2 = 0;
            display_setCursor(0, 0);
            fillScreen(ILI9341_BLACK); 
            hr[i - 1] = 0;
            disp_texto(hr);
            estado_Display = estado_leyendo;
            break;
            
        case estado_imprimiendo:
            fillScreen(ILI9341_BLACK);
            display_setCursor(0, 0);
            display_setTextColor2(ILI9341_WHITE);
            display_setTextSize(2);
            disp_texto("El valor que me llego es: \r\n");


            __delay_ms(100);
          //  display_print(dato1);
            __delay_ms(100);
            estado_Display = estado_leyendo;
            break;    
    }
}
void actmefcora(void){
    switch (estado_Corazon){
        case estado_alto:

            fillRect(258, 182, 8, 4, ILI9341_RED);
            fillRect(282, 182, 8, 4, ILI9341_RED);
            fillRect(254, 186, 16, 4, ILI9341_RED);
            fillRect(278, 186, 16, 4, ILI9341_RED);
            fillRect(250, 190, 48, 12, ILI9341_RED);
            fillRect(254, 202, 40, 4, ILI9341_RED);
            fillRect(258, 206, 32, 4, ILI9341_RED);
            fillRect(262, 210, 24, 4, ILI9341_RED);
            fillRect(266, 214, 16, 4, ILI9341_RED);
            fillRect(270, 218, 8, 4, ILI9341_RED);
            break;    
    }
}
