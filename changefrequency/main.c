#include "stdint.h"
#include "stdbool.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

unsigned char valor_pf = 0x02;
unsigned char sw1_lock = 0x00;
unsigned char sw2_lock = 0x00;
unsigned int  delay_value = 100000;

#define GPIO_PORTF_LOCK_R (*((volatile uint32_t*)0x40025520))
#define GPIO_PORTF_CR_R (*((volatile uint32_t*)0x40025524))

int main(void)
{
    SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_3);

    // Localização das portas
    /*
     Nome da Mask    Binário    Hexadecimal
     GPIO_PIN_0    0b0000 0001  0x00000001  // GPIO pin 0 (SW 2)
     GPIO_PIN_1    0b0000 0010  0x00000002  // GPIO pin 1 (LED vermelho)
     GPIO_PIN_2    0b0000 0100  0x00000004  // GPIO pin 2 (LED azul)
     GPIO_PIN_3    0b0000 1000  0x00000008  // GPIO pin 3 (LED verde)
     GPIO_PIN_4    0b0001 0000  0x00000010  // GPIO pin 4 (SW 1)
     */

    // 1. Habilitando a PF0

    // O primeiro passo é definir as seguintes variáveis no início do código:

    // #define GPIO_PORTF_LOCK_R (*((volatile uint32_t*)0x40025520))
    // #define GPIO_PORTF_CR_R (*((volatile uint32_t*)0x40025524))

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // O segundo passo é empregar os seguintes códigos para desbloqueio da
    // porta PF0, a qual é protegida pela arquitetura do TIVA

    GPIO_PORTF_LOCK_R = 0x4c4f434b;
    GPIO_PORTF_CR_R = 0x1f;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);


    // - Ajuste o tempo de SisCtlDelay conforme o aperto dos botões.
    // - SW1 aumenta e SW2 baixa
    // - O parâmetro deverá variar entre 1.000.000 e 100.000
    // - Deverá haver um LED verde piscando, de forma a indicar a mudança de frequência
    // - A mudança de frequência deverá ocorrer SOMENTE ao pressionar o botão
    // - O SisCtlDelay deve ser aplicado para acender e apagar o LED

    while (1)
      {
           
        if ((!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) && (sw1_lock == 0x00))
        {
            if (delay_value >= 1000000) // testa se a variavel chegou no limite maximo
            {
                delay_value = 1000000; // caso sim, não acresce valor
            }
            else
            {
                delay_value = delay_value + 20000; // sobe a frequencia
            }
            sw1_lock = 0x01;
            SysCtlDelay(50000);
        }
      
        if ((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) && (sw1_lock == 0x01))
        {
            sw1_lock = 0x00;
            SysCtlDelay(50000);
        }

        if ((!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) && (sw2_lock == 0x00))
        {

            if (delay_value <= 100000) // testa se a variavel chegou no limite mínimo
            {
                delay_value = 100000; // caso sim, não diminui valor
            }
            else
            {
                delay_value = delay_value - 20000; // diminui a frequencia
            }

            sw2_lock = 0x01;
            SysCtlDelay(50000);
          
        }
        if ((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) && (sw2_lock == 0x01))
        {
            sw2_lock = 0x00;
            SysCtlDelay(50000);
        }

        SysCtlDelay(delay_value);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,0xff);
        SysCtlDelay(delay_value);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,0x00);

    }
}
