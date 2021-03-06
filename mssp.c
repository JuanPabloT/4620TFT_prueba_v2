/*============================================================================
 * Licencia:
 * Autor:       Leonardo Davico
 * Fecha:       12/10/2020
 *===========================================================================*/
/** @file
 * @brief	Contiene las definiciones del puerto serie sincronico
 * 
 * Aqui se encuentra la implementación del MSSP, los defines, macros, 
 * datos internos y externos, declaraciones y definiciones de funciones
 * internas y definiciones de funciones externas
 */

/*==================[inclusiones]============================================*/
#include <xc.h>         /* Archivo de Encabezados General XC8 */
#include <stdint.h>     /* para las definiciones de uint8_t por ej.*/
#include "system.h"     /* Funciones/Parametros System, como osc/peripheral config */
#include "mssp.h"       /* Funciones/Parametros del mssp */
#include "user.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

/*****************************************************************************/
/*  Funciones SPI                                                            */
/*****************************************************************************/

/**
 * @brief       Configura e inicializa el puerto SPI
 * @param[in]   sType: Modo de funcionamiento del puerto SPI 
 *                  SPI_MASTER_OSC_DIV4     SPI_MASTER_OSC_DIV16
 *                  SPI_MASTER_OSC_DIV64    SPI_MASTER_TMR2
 *                  SPI_SLAVE_SS_EN         SPI_SLAVE_SS_DIS
 * @param[in]   sDataSample: Selecciona el einstante de muestreo
 *                  SPI_DATA_SAMPLE_MIDDLE  SPI_DATA_SAMPLE_END
 * @param[in]   sClockIdle: Selecciona el estado inactivo de clock
 *                  SPI_CLOCK_IDLE_HIGH     SPI_CLOCK_IDLE_LOW 
 * @param[in]   sTransmitEdge: Selecciona flanco de transmisi�n
 *                  SPI_IDLE_2_ACTIVE       SPI_ACTIVE_2_IDLE
 * @return      Nada
 */
void spiInit(   spiType_t           sType, 
                spiDataSample_t     sDataSample, 
                spiClockIdle_t      sClockIdle, 
                spiTransmitEdge_t   sTransmitEdge) {
    TRIS_SDO = 0;               //SDO es salida
    TRIS_SDI = 1;               //SDI es entrada
    if(sType & 0b00000100){     //Si es Slave Mode
        SSPSTAT = sTransmitEdge;
        TRIS_SCK = 1;           //SCK es entrada de clock
    }
    else{                       //Si es Master Mode
        SSPSTAT = sDataSample | sTransmitEdge;
        TRIS_SCK = 0;           //SCK es salida de clock
    }
    SSPCON1 = sType | sClockIdle;
}

static void spiReceiveWait() {
    while ( !SSPSTATbits.BF );  //Espera que se complete la recepcion de un dato
}

/**
 * @brief	    Envia un byte por SPI
 * @param[in]   value Valor a transmitir 
 * @return	    Nada
 * @note        Solo inicia la transmisión, no espera la finalizacion
 */
void spiWriteByte(uint8_t value) {
    SSPBUF = value;
}

/**
 * @brief       Indica si hay un dato disponible
 * @return      1 si hay un byte recibido, 0 si no hay dato disponible 
 */
bit spiDataReady(void) {        //Testea si hay un dato recibido
    if(SSPSTATbits.BF)
        return 1;
    else
        return 0;
}

/**
 * @brief       Recibe de un byte por SPI
 * @return      El byte recibido
 * @note        Espera que finalice la recepción
 */
uint8_t spiReadByte(void) {
    spiReceiveWait();           //Espera que se complete la recepcion de un dato
    return(SSPBUF);             //retorna el byte recibido del buffer
}

/******************************************************************************/
/*  Funciones I2C                                                             */
/******************************************************************************/

/**
 * @brief       Configura e inicializa el puerto I2C
 * @return      Nada
 * @note        Las opciones se modifican en el código, por ejemplo la 
 *              constante I2CFCLOCK
 * @sa          I2CFCLOCK
 */
void i2cInit(void) {
    TRIS_SDI = 1;               //SDI es entrada como informa la data sheet
    TRIS_SCK = 1;               //SCK es entrada como informa la data sheet
    SSPSTAT  = 0b10000000;      //velocidad standar, bus I2C
    SSPADD   = (_XTAL_FREQ/(4*I2C_FCLOCK))-1;//velocidad=Fosc/(4*SSPADD+1)
    SSPCON1   = 0b00101000;      //Modulo SSP en modo I2C Master habilitado
    SSPCON2  = 0b00000000;      //Comunicación I2C no iniciada
}

 /**
 * @brief       Espera fin de alguna de las actividades i2c en curso
 * @return      Nada
 */
void i2cWait(void) {
    while ((SSPCON2 & 0b00011111) || (SSPSTAT & 0b00000100));
}

 /**
 * @brief       Inicia la trama I2C
 * @return      Nada
 */
void i2cStart(void) {
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.SEN = 1;        //inicia la comunicación i2c
}

 /**
 * @brief       Reinicia la trama I2C
 * @return      Nada
 */
void i2cReStart(void) {
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.RSEN = 1;       //reinicia la comunicación i2c
}

 /**
 * @brief       Termina la trama I2C
 * @return      Nada
 */
void i2cStop(void) {
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.PEN = 1;        //detiene la comunicación i2c
}

/**
 * @brief       Transmite un byte del maestro al esclavo
 * @param[in]   value Valor a transmitir 
 * @return      0 si la transmisión es correcta 
 */
bit i2cWriteByte(uint8_t value) {
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPBUF = value;             //cargar en el registro SSPBUF el dato a enviar
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    return  SSPCON2bits.ACKSTAT ? 1 : 0;//devuelve un 0 si la transmisión es correcta
}

/**
 * @brief       Lee un byte del puerto I2C, enviados por el esclavo al maestro
 * @return      El byte recibido
 * @note        Se debe terminar la lectura i2cReadAck para continuar con mas 
 *              lecturas  o con i2cReadNoAck si es la ultima
 * @sa          i2cReadAck, i2cReadNoAck
 */
uint8_t i2cReadByte(void) {
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.RCEN = 1;       //reinicia la comunicación i2c, ya que la comunicación
                                //se ha tenido que iniciar antes, para enviar la
                                //dirección del esclavo y del registro a leer
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    return SSPBUF;              //Retorna dato recibido 
}

/**
 * @brief       Se utiliza despues de i2cReadByte para indicar que se leeran mas datos
 * @return      Nada
 * @sa          i2cReadByte
 */
void i2cReadAck(void) {
    SSPCON2bits.ACKDT = 0;      //el maestro lo pone a 0 después de recibir el dato,
                                //para indicarle al esclavo que se leeran mas datos
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.ACKEN = 1;      //inicia secuencia final de reconocimiento 
                                //enviando al esclavo el valor de ACKDT
}

/**
 * @brief       Se utiliza despues de i2cReadByte para indicar que no se leeran mas datos
 * @return      Nada
 * @sa          i2cReadByte
 */
void i2cReadNoAck(void) {
    SSPCON2bits.ACKDT = 1;      //el maestro lo pone a 1 después de recibir el dato,
                                //para indicarle al esclavo que ya no se leeran
                                //mas datos
    i2cWait();                  //espera fin de alguna actividades i2c en curso
    SSPCON2bits.ACKEN = 1;      //inicia secuencia final de reconocimiento 
                                //enviando al esclavo el valor de ACKDT
}
//void errEnSHT21(void){
//    PIN_LED_V = 0;
//    PIN_LED_R = 1;
//    printf("Error en SHT21");
//    while(1);
//}

/*==================[fin del archivo]========================================*/


//void SPI1_Initialize(void)
//{
//    //Setup PPS Pins
//    SSP1CLKPPS = 19;
//    SSP1DATPPS = 20;
//    RC3PPS    = 20;
//    RC6PPS    = 21;
//    //SPI setup
//    SSP1STAT = 0xC0;
//    SSP1CON1 = 0x00;
//    SSP1ADD = 0x01;
//    TRISCbits.TRISC3 = 0;
//    SSP1CON1bits.SSPEN = 0;
//}