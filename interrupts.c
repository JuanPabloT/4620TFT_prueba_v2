/******************************************************************************/
/* Archivos a Incluir                                                         */
/******************************************************************************/
#include <xc.h>             /* Archivo de Encabezados General XC8 */
#include "user.h"

/******************************************************************************/
/* Routinas de Interrupción                                                   */
/******************************************************************************/
void __interrupt() isr(void);

void __interrupt() isr(void) {
    /* El sig. ejemplo de codigo muestra el manejo de interrupciones.
    Note que no son bloques if separados sino que son if else ecadenados.
    No use bloques if separados para cada flag de interrupción para evitar
    errores en tiempo de ejecución. */
    /* TODO: Complete con el codigo de las rutinas de interrupción aquí. */
    /* Determine que flag ha generado la interrupción */
    if (TMR0IF) {
        TMR0IF = 0; // Bajo Interrupt Flag
        TMR0 += 7;
        tickCounter++;
        
    
    } 
    
}