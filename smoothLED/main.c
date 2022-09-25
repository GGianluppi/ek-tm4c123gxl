// Blue, Red and Green

#include "stdint.h"
#include "stdbool.h"
#include "inc/tm4c123gh6pm.h" 
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.c"
#include "driverlib/timer.h"
#include "driverlib/timer.c"
#include "driverlib/pwm.h"
#include "driverlib/pwm.c"
#include "driverlib/pin_map.h"

#define FREQUENCIA_G2 10000
#define FREQUENCIA_G3 10000

unsigned int PERIODO_G2;
unsigned int PERIODO_G3;
unsigned int LARGURA_PULSO_PWM5;
unsigned int LARGURA_PULSO_PWM6;
unsigned int LARGURA_PULSO_PWM7;

float k = 0.01;

int main(void)
{
    SysCtlClockSet( SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_3);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIO_PORTF_LOCK_R = 0x4c4f434b;
    GPIO_PORTF_CR_R = 0x1f;

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);// ativar botões sw1 sw2
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
  
    // Definições das portas GPIO PF1 PF2 e PF#

    //Passo 1 definir o clock e habilitar

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);//Determina o clock do sisteme PWM como sendo igual ao processador
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); //Habilita o clock para o modulo 1 do PWM

    //Passo 2 Cálculo dos periodos (ciclos de clock) e das larguras do pulso
    PERIODO_G2 = SysCtlClockGet()/FREQUENCIA_G2; // O gerador G2 trabalha com PWM 5
    PERIODO_G3 = SysCtlClockGet()/FREQUENCIA_G3; //O gerador G3 trabalha com PWM 6 e PWM 7

    LARGURA_PULSO_PWM5 = PERIODO_G2*0.01; //Este sinal PWM está conectado ao led vermelho
    LARGURA_PULSO_PWM6 = PERIODO_G2*0.9; //Este sinal PWM está conectado ao led azul
    LARGURA_PULSO_PWM7 = PERIODO_G2*0.9; //Este sinal PWM está conectado ao led verde


    //Passo 3 Configura a PF1, PF2 e PF3 como PWM

    GPIOPinTypePWM (GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //Configura como PWM

    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    //Passo 4 Configura o funcionamento dos geradores PWM

    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_GEN_NO_SYNC |PWM_GEN_MODE_DBG_RUN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_GEN_NO_SYNC |PWM_GEN_MODE_DBG_RUN);


    //Passo 5 Configura o periodo, largura de pulso e habilita o funcionamento

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, PERIODO_G2); //determina periodo de M1 G2
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM5);//determina largura de pulso no sinal PWM 5
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true); //habilita o sinal PWM 5 na saida

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PERIODO_G3); //determina periodo de M1 G3
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM6);//determina largura de pulso no sinal PWM 6
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM7);//determina largura de pulso no sinal PWM 7
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT| PWM_OUT_7_BIT, true); //habilita o sinal PWM 5 na saida

    //Passo 6 liga o Gerador 2 e o gerador 3 de PWM no M1
    PWMGenEnable(PWM1_BASE,PWM_GEN_2);
    PWMGenEnable(PWM1_BASE,PWM_GEN_3);

    while(1)
    {

      // Blue
      SysCtlDelay(500000);
      PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true); // Habilita PWM6

      for(k=0.01; k<1; k = k + 0.01)
      {
         LARGURA_PULSO_PWM6 = PERIODO_G3 * k;  
         PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM6);
         SysCtlDelay(200000);
      }

      for(k=1; k>0.01; k = k - 0.01)
      {
         LARGURA_PULSO_PWM6 = PERIODO_G3 * k;  
         PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, LARGURA_PULSO_PWM6);
         SysCtlDelay(200000);
      }

      PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);

     // Red
     SysCtlDelay(500000);
     PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true); // Habilita PWM5

     for(k=0.01; k<1; k = k + 0.01)
     {
       LARGURA_PULSO_PWM5 = PERIODO_G2 * k;
       PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM5);
       SysCtlDelay(200000);
     }

     for(k=1; k>0.01; k = k - 0.01)
     {
       LARGURA_PULSO_PWM5 = PERIODO_G2 * k;
       PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, LARGURA_PULSO_PWM5);
       SysCtlDelay(200000);
     }

     PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, false);

    // Green
    SysCtlDelay(500000);
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true); // Habilita PWM7

    for(k=0.01; k<1; k = k + 0.01)
    {
       LARGURA_PULSO_PWM7 = PERIODO_G3 * k;
       PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM7);
       SysCtlDelay(200000);
    }

    for(k=1; k>0.01; k = k - 0.01)
    {
        LARGURA_PULSO_PWM7 = PERIODO_G3 * k;
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, LARGURA_PULSO_PWM7);
        SysCtlDelay(200000);
    }

    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, false);

    } 

}
