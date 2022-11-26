/*
 * oled_096.h
 *
 *  Created on: May 15, 2021
 *      Author: cszhao
 */

#ifndef OLED_096_H_
#define OLED_096_H_

#include "stm32f0xx_hal.h"

uint8_t oled_init(I2C_HandleTypeDef *hi2c);
uint8_t oled_clear(I2C_HandleTypeDef *hi2c);
void oled_clear_line(I2C_HandleTypeDef *hi2c, uint8_t line);
void oled_paint_char(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, uint8_t c);
void oled_paint_string(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, char *str);
void oled_paint_hz(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, uint8_t hz_id);

#endif /* INC_OLED_096_H_ */
