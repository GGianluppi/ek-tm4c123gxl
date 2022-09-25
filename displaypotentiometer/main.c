#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "inc/tm4c123gh6pm.h"    // Definições para interrupções e registradores do Tiva C Series
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"    // Coloca-se o .c para debug
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"      // Debug das GPIOs
#include "driverlib/interrupt.h" // Definições e macros para as API do controlador NVIC
#include "driverlib/interrupt.c" // Debug das interrupções
#include "driverlib/timer.h"     // Definições e macros para as API do timer
#include "driverlib/timer.c"     // Debug do timer
#include "driverlib/pwm.h"       // Definições e macros para as API do PWM
#include "driverlib/pwm.c"
#include "driverlib/pin_map.h"   // Mapa das definições dos periféricos e suas respectivas funções
#include "driverlib/adc.h"       // Definições e macros para as API do conversor A/D
#include "driverlib/adc.c"       // Debug do conversor A/D

unsigned int primeira_conv = 1;
char tensao_display[2];

unsigned int valor_adc;
float k_adc = 0.00080586080586081; //k_adc = 4095/3.3
float tensao = 0.0;

//interrupção do conversor AD
void ADC0IntHandler(void);
void lcd_init(void);
void display_lcd(unsigned char);
void lcd_data(unsigned char);

void ADC0IntHandler(void)
{
    //limpa flag da interrupção no sequenciador 3
    ADCIntClear(ADC0_BASE, 3);
    if(primeira_conv)
    {
        lcd_init();
        Lcd_String_Display("Tensao: ");
        primeira_conv = 0;
    }
    //lê tensão na entrada analógica configurada e armazena na variável valor_adc
    ADCSequenceDataGet(ADC0_BASE, 3, &valor_adc);
    //tensao em volts lida
    tensao = k_adc * valor_adc;
    ADCProcessorTrigger(ADC0_BASE, 3);
    //transforma tensao lida em float para texto, armazenando na variável tensao_display

    if(tensao > 0 && tensao < 1)
    {
        if(tensao < 0.5)
        {
            Lcd_String_Display("0.0V");
        }
        else if(tensao > 0.5)
        {
            Lcd_String_Display("0.5V");
        }
    }
    else if(tensao > 1 && tensao < 2)
    {
            if(tensao < 1.5)
            {
                Lcd_String_Display("1.0V");
            }
            else if(tensao > 1.5)
            {
                Lcd_String_Display("1.5V");
            }
    }
    else if(tensao > 2)
    {
        if(tensao < 2.5)
        {
            Lcd_String_Display("2.0V");
        }
        else if(tensao > 2.5 && tensao < 3)
        {
            Lcd_String_Display("2.5V");
        }
        else
        {
            Lcd_String_Display("3.0V");
        }
    }
    SysCtlDelay(SysCtlClockGet()/10);
    display_LCD(0b0001);
    display_LCD(0b0000);
    SysCtlDelay(SysCtlClockGet()/10);
    display_LCD(0b0001);
    display_LCD(0b0000);
    SysCtlDelay(SysCtlClockGet()/10);
    display_LCD(0b0001);
    display_LCD(0b0000);
    SysCtlDelay(SysCtlClockGet()/10);
    display_LCD(0b0001);
    display_LCD(0b0000);
    SysCtlDelay(SysCtlClockGet()/10);

}

void display_LCD (unsigned char dados)
{
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0x00);

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, (dados<<4));
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0xff);//habilita lcd pulso alto para pulso baixo
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0x00);
}

void lcd_data (unsigned char dados)
{
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0x00);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_2, 0xff);

    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, dados);
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0xff); //habilita lcd pulso alto para pulso baixo
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0x00);
    SysCtlDelay(SysCtlClockGet()/100);

    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, (dados<<4));
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0xff); //habilita lcd pulso alto para pulso baixo
    SysCtlDelay(SysCtlClockGet()/100);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0, 0x00);
    SysCtlDelay(SysCtlClockGet()/100);
}


void lcd_init()
{
    SysCtlDelay(SysCtlClockGet()/1);
    //function set 4 bits
    display_LCD(0b0010);
    display_LCD(0b0010);
    display_LCD(0b1100);
    SysCtlDelay(SysCtlClockGet()/1);
    //display on/off
    display_LCD(0b0000);
    display_LCD(0b1100);
    SysCtlDelay(SysCtlClockGet()/1);
    //display clear
    display_LCD(0b0000);
    display_LCD(0b0001);
    SysCtlDelay(SysCtlClockGet()/1);
    //entry mode set
    display_LCD(0b0000);
    display_LCD(0b0110);
    SysCtlDelay(SysCtlClockGet()/1);

}

//escreve, caractere a caractere, dados no display
void Lcd_String_Display(unsigned char *str)
{
    while (*str)
    {
        lcd_data(*str);
        str++;
    }
}

int main(void)
{
    SysCtlClockSet( SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_SYSDIV_1);

    //habilitação de periféricos para acionamento do display LCD em 4 bits e periférico do PWM (PE4)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //habilitação de periféricos do conversor AD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    //Configuração do display
    //barramento de dados 4 bits
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    //barramento de enable e RS
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    //Configuração PWM
    //PWM funcionará com mesmo clock do processador
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);
    //Selecionando Módulo 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    //definindo PE4 como PWM
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
    //PE4 corresponderá ao PWM2 do módulo 1
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    //PWM2 depende do gerador 1, configurando-o: gerador no modo descida, assíncrono e continua rodando no modo debug
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC | PWM_GEN_MODE_DBG_RUN);
    //definindo período dos PWMs do gerador 1 (T = clock_processador/frequência). Operando em 10kHz
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, SysCtlClockGet()/10000);
    //trigger no "zero" do gerador 1, para acionamento do AD
    PWMGenIntTrigEnable(PWM1_BASE, PWM_GEN_2, PWM_TR_CNT_ZERO);
    //habilita gerador 1
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    //habilita PWM2 no gerador 1
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

    //Configuração AD
    //Habilita canal de conversão ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //Define a frequência máxima permitida para conversão
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL, 1);
    //Definindo PD2 como entrada analógica
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    //Configurando AD para trabalhar com referência interna
    ADCReferenceSet(ADC0_BASE, ADC_REF_INT);
    //Desabilitando sequenciador 3 antes de configurá-lo, conforme recomendação da TEXAS. Utiliza-se o sequenciador 3 porque trabalha com uma amostra por conversão
    ADCSequenceDisable(ADC0_BASE, 3);
    //configura prioridade do sequenciador 3 e origem do trigger (módulo | PWM)
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    //configuração dos passos da conversão, da seguinte forma:
    //canal 0, sequenciador 3, primeiro passo (0). Começa na entrada 5 (associada a porta pd2)
    //gera uma interrupção de fim de conversão e espera o próximo trigger
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH5 | ADC_CTL_IE | ADC_CTL_END);
    //define rotina de interrupção
    ADCIntRegister(ADC0_BASE, 3, ADC0IntHandler);
    //Habilita interrupção no canal 0, sequenciador 3
    ADCIntEnable(ADC0_BASE, 3);
    //Habilita o sequenciador 3
    ADCSequenceEnable(ADC0_BASE, 3);

    IntMasterEnable();
    //habilita todas as interrupções


    ADCProcessorTrigger(ADC0_BASE, 3);

    while(1)
    {
    } // Fecha while
} // Fecha main


