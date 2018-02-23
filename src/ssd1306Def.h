/**
  ******************************************************************************
  * @file    ssd1306Def.h
  * @author  Gerasimchuk A.
  * @version V1.0.0
  * @date    7-October-2017
  * @brief
  */



#ifndef SSD1306DEF_H_
#define SSD1306DEF_H_

#define SSD1306_I2C_ADDRESS_1 0b0111100
#define SSD1306_I2C_ADDRESS_2 0b0111101

#define BIT_DC_COMAND    0b00000000
#define BIT_DC_DATA      0b01000000

#define BIT_C_CONTONUE   0b10000000
#define BIT_C_SIMPLE     0b00000000

#define CONTROL_COMAND_SIMLE 0b00000000
#define CONTROL_COMAND_CONT  0b10000000

#define CONTROL_DATA_SIMLE 0b01000000
#define CONTROL_DATA_CONT  0b11000000





typedef enum {
    setDisplayOnOff_OFF = 0xAE,
    setDisplayOnOff_ON  = 0xAF
}setDisplayOnOffDef;

typedef enum {
    entireDisplayOn_ON = 0xA4,
    entireDisplayOn_Resume  = 0xAF
}entireDisplayOnOffDef;



#endif
