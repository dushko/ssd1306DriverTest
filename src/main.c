#include "stdint.h"
#include "stdbool.h"

#include "system_stm32f4xx.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_tim.h"

#include "stm32f4xx_conf.h"
#include "stm32f4xx.h"

#include "ssd1306Def.h"

/*ORANGE LED DEF*/
#define LED_ORANGE_GPIO_PORT GPIOD
#define LED_ORANGE_GPIO_PIN  GPIO_PinSource13

/*BLUE LED DEF*/
#define LED_BLUE_GPIO_PORT   GPIOD
#define LED_BLUE_GPIO_PIN    GPIO_PinSource15

/*GREEN LED DEF*/
#define LED_GREEN_GPIO_PORT  GPIOD
#define LED_GREEN_GPIO_PIN   GPIO_PinSource12

/*RED LED DEF*/
#define LED_RED_GPIO_PORT    GPIOD
#define LED_RED_GPIO_PIN     GPIO_PinSource14

/*TIM definition*/
#define SEL_TIM   TIM2
#define PERIOD    50

/*I2C definition*/
#define SEL_I2C           I2C1
#define I2C_FRQ_Hz       400000
/*I2C GPIO definition*/
#define I2C_DATA_PORT     GPIOB
#define I2C_DATA_PIN      GPIO_PinSource7

#define I2C_SCK_PORT      GPIOB
#define I2C_SCK_PIN       GPIO_PinSource6
// timeout on I2C operation
#define I2C_TIMEOUT       1000





#define SSD1306_LCDWIDTH      128
#define SSD1306_LCDHEIGHT      64
#define SSD1306_SETCONTRAST   0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

#define SSD1306_PAGE_START     0b10110000
#define SSD1306_ROW_START_LO   0b00000000
#define SSD1306_ROW_START_HI   0b00010000

#define SSD1306_PAGE_START_END 0b00100010

typedef enum{
    I2C_REZ_OK,
    I2C_REZ_ERROR
}I2C_REZ_DEF;


typedef enum{
    ORANGE_LED,
    GREEN_LED,
    BLUE_LED,
    RED_LED,
}ledColor;


typedef struct {
    GPIO_TypeDef *gpioPORT;
    uint16_t      gpioPIN;
}gpioDef;


gpioDef gpioList[] = {
        [ORANGE_LED] = {
            .gpioPORT = LED_ORANGE_GPIO_PORT,
            .gpioPIN  = LED_ORANGE_GPIO_PIN
        },
        [GREEN_LED] = {
            .gpioPORT = LED_GREEN_GPIO_PORT,
            .gpioPIN  = LED_GREEN_GPIO_PIN
        },
        [RED_LED] = {
            .gpioPORT = LED_RED_GPIO_PORT,
            .gpioPIN  = LED_RED_GPIO_PIN
        },
        [BLUE_LED] = {
            .gpioPORT = LED_BLUE_GPIO_PORT,
            .gpioPIN  = LED_BLUE_GPIO_PIN
        }
    };



void enableGpio(GPIO_TypeDef *gpioEnable){
    if(gpioEnable == GPIOA)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    }
    if(gpioEnable == GPIOB)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    }
    if(gpioEnable == GPIOC)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    }
    if(gpioEnable == GPIOD)
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    }
}



void initGpio(void){
    GPIO_InitTypeDef gpioInit;
    uint8_t cnt = 0;
    for(; cnt < sizeof(gpioList)/sizeof(gpioList[0]); cnt++)
    {
        enableGpio(gpioList[cnt].gpioPORT);

        gpioInit.GPIO_Mode = GPIO_Mode_OUT;
        gpioInit.GPIO_Pin =  ( 1<<gpioList[cnt].gpioPIN );
        gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
        gpioInit.GPIO_OType = GPIO_OType_PP;
        gpioInit.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(gpioList[cnt].gpioPORT, &gpioInit);
       // GPIO_SetBits(gpioList[cnt].gpioPORT, gpioList[cnt].gpioPIN );
    }
    GPIO_ToggleBits(gpioList[ORANGE_LED].gpioPORT, gpioList[ORANGE_LED].gpioPIN );
}

void timInit(){
    RCC_ClocksTypeDef rccClock;
    TIM_TimeBaseInitTypeDef timImit;
    RCC_GetClocksFreq(&rccClock);
    uint32_t fAPB = rccClock.PCLK2_Frequency/1000000;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    //timImit.TIM_ClockDivision = TIM_ClockDivision;
    timImit.TIM_CounterMode = TIM_CounterMode_Up;
    timImit.TIM_Prescaler =  fAPB * 100;
    timImit.TIM_Period = PERIOD*10;
    timImit.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(SEL_TIM, &timImit);

    TIM_ITConfig(SEL_TIM, TIM_IT_Update, ENABLE);

    TIM_ARRPreloadConfig(SEL_TIM, ENABLE);

    NVIC_EnableIRQ(TIM2_IRQn);
    TIM_Cmd(SEL_TIM, ENABLE);
}


void TIM2_IRQHandler(void){
    static uint8_t currenLED = 0;
    TIM_ClearFlag(SEL_TIM, TIM_IT_Update);

    GPIO_ToggleBits(gpioList[currenLED].gpioPORT, 1<<gpioList[currenLED].gpioPIN );
    currenLED++;
    if(currenLED >= sizeof(gpioList)/sizeof(gpioList[0])){
        currenLED = 0;
    }
    GPIO_ToggleBits(gpioList[currenLED].gpioPORT, 1<<gpioList[currenLED].gpioPIN );

}

void delay(uint32_t delay)
{
    uint32_t dalayIn = delay;
    uint32_t cnt = 0;
    while(cnt<dalayIn)
    {
        cnt++;
    }
}

void initI2C(void){
 /*INIT GPIO*/
    GPIO_InitTypeDef gpioInit;
    enableGpio(I2C_SCK_PORT);
    enableGpio(I2C_DATA_PORT);
    /*Config DATA*/
    gpioInit.GPIO_Mode = GPIO_Mode_AF;
    gpioInit.GPIO_Pin =  1<<I2C_SCK_PIN;
    gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
    gpioInit.GPIO_OType = GPIO_OType_OD;
    gpioInit.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C_SCK_PORT, &gpioInit);
    GPIO_PinAFConfig(I2C_SCK_PORT, I2C_SCK_PIN, GPIO_AF_I2C1);

    /*Config SCK*/
    gpioInit.GPIO_Mode = GPIO_Mode_AF;
    gpioInit.GPIO_Pin =  1<<I2C_DATA_PIN;
    gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
    gpioInit.GPIO_OType = GPIO_OType_OD;
    gpioInit.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(I2C_DATA_PORT, &gpioInit);
    GPIO_PinAFConfig(I2C_DATA_PORT, I2C_DATA_PIN, GPIO_AF_I2C1);
    /*I2C INIT*/

    I2C_InitTypeDef i2cInit;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    i2cInit.I2C_Ack = I2C_Ack_Enable;
    i2cInit.I2C_ClockSpeed = I2C_FRQ_Hz;
    i2cInit.I2C_DutyCycle = I2C_DutyCycle_2;
    i2cInit.I2C_Mode = I2C_Mode_I2C;
    i2cInit.I2C_OwnAddress1 = 0b1111111;
    I2C_Init(SEL_I2C, &i2cInit);
    I2C_Cmd(SEL_I2C, ENABLE);
}

uint32_t time[100];
uint8_t cnt1;

/*
    outState - expected state
*/
I2C_REZ_DEF rezOperationI2C(I2C_TypeDef *inI2C, uint32_t eventI2C, FlagStatus outStatus)
{
    uint32_t cnt = 0;
    while( I2C_GetFlagStatus(inI2C,eventI2C) != outStatus )
    {
        if(cnt >= I2C_TIMEOUT)
            return(I2C_REZ_ERROR);
        cnt++;
    }

    time[cnt1] = cnt;
    cnt1++;
    return I2C_REZ_OK;
}

I2C_REZ_DEF sendDisplayData(uint8_t *data, uint16_t numData){
    uint16_t cnt = 0;

    I2C_GenerateSTART(SEL_I2C, ENABLE);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_SB, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    I2C_Send7bitAddress(SEL_I2C, SSD1306_I2C_ADDRESS_1<<1, I2C_Direction_Transmitter);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_ADDR, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    (void)I2C1->SR2;


    I2C_SendData(SEL_I2C, CONTROL_DATA_SIMLE);
    if(rezOperationI2C(SEL_I2C, I2C_FLAG_TXE, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    do{

        I2C_SendData(SEL_I2C, data[cnt]);

        if(rezOperationI2C(SEL_I2C, I2C_FLAG_TXE, SET) == I2C_REZ_ERROR)
            return I2C_REZ_ERROR;

    }while(++cnt < numData);


    I2C_GenerateSTOP(SEL_I2C, ENABLE);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_STOPF, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    return I2C_REZ_OK;

return I2C_REZ_OK;
}

I2C_REZ_DEF sendDisplayConfig(uint8_t *data, uint8_t numData){
    uint8_t cnt = 0;
    I2C_GenerateSTART(SEL_I2C, ENABLE);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_SB, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    I2C_Send7bitAddress(SEL_I2C, SSD1306_I2C_ADDRESS_1<<1, I2C_Direction_Transmitter);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_ADDR, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    (void)I2C1->SR2;


    I2C_SendData(SEL_I2C, CONTROL_COMAND_SIMLE);
    if(rezOperationI2C(SEL_I2C, I2C_FLAG_TXE, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    do{

        I2C_SendData(SEL_I2C, data[cnt]);

        if(rezOperationI2C(SEL_I2C, I2C_FLAG_TXE, SET) == I2C_REZ_ERROR)
            return I2C_REZ_ERROR;

    }while(++cnt < numData);

    I2C_GenerateSTOP(SEL_I2C, ENABLE);

    if(rezOperationI2C(SEL_I2C, I2C_FLAG_STOPF, SET) == I2C_REZ_ERROR)
        return I2C_REZ_ERROR;

    return I2C_REZ_OK;
}

uint8_t clearDisplay[4*128] = { [0 ... (4*128 - 1)] = 0 };

uint8_t firstData1[] = {
     0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b10010010, 0b01010100, 0b00111000, 0b00010000

};
    /*
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
*/

// 0,    0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 //0,    0,    0xFF, 0xFF, 0xFF, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,};
 //0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

 /*0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0xFF,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };*/


uint8_t comandBuff[3];

void updateScreen(void){
    sendDisplayData(clearDisplay, sizeof(clearDisplay));
    sendDisplayData(firstData1, sizeof(firstData1));

}

int main(void)
{



    SystemInit();

    initGpio();
    timInit();
    initI2C();

    // Init sequence for 128x64 OLED module
    comandBuff[0] = SSD1306_DISPLAYOFF;          //+
    sendDisplayConfig(comandBuff,1);                    // 0xAE

    comandBuff[0] = SSD1306_SETDISPLAYCLOCKDIV; //+
    comandBuff[1] = 0x80;
    sendDisplayConfig(comandBuff,2);             // 0xD5

    comandBuff[0] = SSD1306_SETMULTIPLEX;     // 0xA8
    comandBuff[1] = 0x20;
    sendDisplayConfig(comandBuff,2);

    comandBuff[0] = SSD1306_SETDISPLAYOFFSET;
    comandBuff[1] = 0x0;
    sendDisplayConfig(comandBuff,2);                                 // no offset

    comandBuff[0] = SSD1306_SETSTARTLINE;    //+
    sendDisplayConfig(comandBuff,1);// | 0x0);        // line #0

    comandBuff[0] = SSD1306_CHARGEPUMP;     //+
    comandBuff[1] = 0x14;
    sendDisplayConfig(comandBuff,2);                    // 0x8D
    //sendDisplayConfig(0x14);              // using internal VCC

    comandBuff[0] = SSD1306_MEMORYMODE;    //++
    comandBuff[1] = 0x00;
    sendDisplayConfig(comandBuff,2);
    //sendDisplayConfig(0x00);          // 0x00 horizontal addressing

    comandBuff[0] = SSD1306_SEGREMAP | 0x0;        //A0
    sendDisplayConfig(comandBuff,1); // rotate screen 180

    comandBuff[0] = SSD1306_COMSCANINC;  //C0
    sendDisplayConfig(comandBuff,1); // rotate screen 180

    comandBuff[0] = SSD1306_SETCOMPINS;  //DA
    comandBuff[1] = 0x00;                                          // !!!!!!!!!!!!!!!!!!!!! My configure !!
    sendDisplayConfig(comandBuff,2);                   // 0xDA
    //sendDisplayConfig(0x12);

    comandBuff[0] = SSD1306_SETCONTRAST;
    comandBuff[1] = 0xF0;
    sendDisplayConfig(comandBuff,2);                  // 0x81
    //sendDisplayConfig(0xCF);

    comandBuff[0] = SSD1306_SETPRECHARGE;   //D9
    comandBuff[1] = 0xF1;
    sendDisplayConfig(comandBuff,2);                  // 0xd9
    //sendDisplayConfig(0xF1);

    comandBuff[0] = SSD1306_SETVCOMDETECT;  //DB
    comandBuff[1] = 0x40;
    sendDisplayConfig(comandBuff,2);                // 0xDB
   // sendDisplayConfig(0x40);

    comandBuff[0] = SSD1306_DISPLAYALLON_RESUME;
    sendDisplayConfig(comandBuff,1);           // 0xA4

    comandBuff[0] = SSD1306_NORMALDISPLAY;
    sendDisplayConfig(comandBuff,1);                 // 0xA6

    comandBuff[0] = SSD1306_DISPLAYON;
    sendDisplayConfig(comandBuff,1);
//------------------------------------------------------------------------------
    comandBuff[0] = SSD1306_PAGE_START | 0;
    sendDisplayConfig(comandBuff,1);



    comandBuff[0] = SSD1306_ROW_START_LO | 0;
    sendDisplayConfig(comandBuff,1);                    //switch on OLED

    comandBuff[0] = SSD1306_ROW_START_HI | 0b1;
    sendDisplayConfig(comandBuff,1);                    //switch on OLED

    comandBuff[0] = SSD1306_PAGE_START_END;
    comandBuff[1] = 0;
    comandBuff[2] = 3;
    sendDisplayConfig(comandBuff,3);                    //switch on OLED

    //sendDisplayData(firstData2, sizeof(firstData2));

    __enable_irq();

    while(1)
    {
        delay(400000);
        updateScreen();
    }
}
