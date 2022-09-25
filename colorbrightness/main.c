#include "stdint.h"
#include "stdbool.h"
#include "inc/tm4c123gh6pm.h" // Definições para interrupções e registradores do Tiva C Series
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/pwm.h"       // Definições e macros para as API do PWM
#include "driverlib/pwm.c"
#include "driverlib/pin_map.h"   // Mapa das definições dos periféricos e suas respectivas funções

#define F_PWM  1000

unsigned char COR = 1;
unsigned int PERIODO_G;
unsigned int LARGURA_PULSO_PWM;

float RAZAO_CICLICA = 0.1;

int main(void)
{
    SysCtlClockSet( SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_64);

    //  Definições das portas GPIO PF0 e PF4

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIO_PORTF_LOCK_R = 0x4c4f434b;
    GPIO_PORTF_CR_R = 0x1f;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //  Definições das portas GPIO PF1, PF2 e PF3

    // Passo 1) Definir o clock e habilitar
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);         // Determina o clock do sistema PWM como sendo igual ao do processador
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // Habilita o clock para o Módulo 1 do PWM

    // Passo 2) Cálculo dos períodos (ciclos de clock) e das larguras de pulso(ciclos de clock)
    PERIODO_G = SysCtlClockGet()/F_PWM; // O Gerador G2 trabalha com o PWM 5
    LARGURA_PULSO_PWM = PERIODO_G*RAZAO_CICLICA;  // Este sinal PWM está conectado no LED vermelho

    // Passo 3) Configura e mapeia a PF1, PF2 e PF3 como PWM
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // Configura como PWM

    GPIOPinConfigure(GPIO_PF1_M1PWM5); // Mapeia PF1 para M1 G2 PWM 5
    GPIOPinConfigure(GPIO_PF2_M1PWM6); // Mapeia PF2 para M1 G3 PWM 6
    GPIOPinConfigure(GPIO_PF3_M1PWM7); // Mapeia PF3 para M1 G3 PWM 7

    // Passo 4) Configura o funcionamento dos geradores PWM
    // Configura M1G2 e M1G3 como um contador DOWN, sem sincronismo, com continuação de funcionamento no modo debug
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_GEN_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_GEN_NO_SYNC | PWM_GEN_MODE_DBG_RUN);

    // Passo 5) Configura o Período, Largura de Pulso e habilita o funcionamento
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, PERIODO_G);          // Determina o período de M1 G2
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM); // Determina a largura de pulso no sinal PWM 5
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);             // Habilita o sinal PWM 5 na saída

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PERIODO_G);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);

    // Passo 6) Liga o Gerador 2 e o Gerador 3 de PWM no Módulo 1
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);


    while(1)
    {

        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) // Leitura do botão para mudança de razão cíclica
        {
            RAZAO_CICLICA = RAZAO_CICLICA + 0.0001;

            if(RAZAO_CICLICA >= 1)
            {
                RAZAO_CICLICA = 0.01;
            }


            LARGURA_PULSO_PWM = PERIODO_G*RAZAO_CICLICA;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM);
        }

        if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))
        {

            switch(COR)
            {
            case 1:
                PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
                PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT|PWM_OUT_7_BIT, false);
                break;
            case 2:
                PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
                PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_7_BIT, false);
                break;
            case 3:
                PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
                PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT, false);
                break;
            }

            COR++; // Sem ele so fica no vermelho

            if(COR >= 4)
            {
                COR = 1;
            }

            RAZAO_CICLICA = 0.01;
            LARGURA_PULSO_PWM = PERIODO_G*RAZAO_CICLICA;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM);
        }
    } // Fecha while
} // Fecha main
