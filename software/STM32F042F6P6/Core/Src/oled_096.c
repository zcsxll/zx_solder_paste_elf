/*
 * oled_096.c
 *
 *  Created on: May 15, 2021
 *      Author: cszhao
 */

#include "oled_096.h"
#include "font_8x16.h"

//0xA1和0xA0是左右和右左，0xC0和0xX8是上下和下上
uint8_t CONF_DATA[] = { 0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA0, 0xA6,
                        0xA8, 0x3F, 0xC0, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA,
                        0x12, 0xD8, 0x30, 0x8D, 0x14, 0xAF
                      };

uint8_t oled_write_cmd(I2C_HandleTypeDef *hi2c, uint8_t cmd) {
    return HAL_I2C_Mem_Write(hi2c, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 0x100);
}

uint8_t oled_write_data(I2C_HandleTypeDef *hi2c, uint8_t data) {
    return HAL_I2C_Mem_Write(hi2c, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100);
}

uint8_t oled_init(I2C_HandleTypeDef *hi2c) {
    HAL_Delay(500);
    for(uint8_t i = 0; i < 27; i++) {
        if(oled_write_cmd(hi2c, CONF_DATA[i])) {
            return 1;
        }
    }

    return 0;
}

void oled_clear_line(I2C_HandleTypeDef *hi2c, uint8_t line) {
    oled_write_cmd(hi2c, 0xb0 + line);
    oled_write_cmd(hi2c, 0x00);
    oled_write_cmd(hi2c, 0x10);
    for(uint8_t n = 0; n < 128; n++) {
        oled_write_data(hi2c, 0);
    }
}

uint8_t oled_clear(I2C_HandleTypeDef *hi2c) {
    uint8_t ret;
    for(uint8_t i = 0; i < 8; i++) {
        ret = oled_write_cmd(hi2c, 0xb0 + i);
        ret += oled_write_cmd(hi2c, 0x00);
        ret += oled_write_cmd(hi2c, 0x10);
        for(uint8_t n = 0; n < 128; n++)
            oled_write_data(hi2c, 0);
    }

    return ret;
}

void oled_set_pos(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y) {
    oled_write_cmd(hi2c, 0xb0 + y);
    oled_write_cmd(hi2c, ((x & 0xf0) >> 4) | 0x10);
    oled_write_cmd(hi2c, x & 0x0f);
}

//x:0~127, y:0~63
void oled_paint_char(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, uint8_t c) {
    c -= ' '; //得到偏移后的值
    if(x > 128 - 1) {
        x = 0;
        y = y + 2;
    }

    oled_set_pos(hi2c, x, y);
    for(uint8_t i = 0; i < 8; i++) {
        oled_write_data(hi2c, F8x16[c * 16 + i]);
    }
    oled_set_pos(hi2c, x, y + 1);
    for(uint8_t i = 0; i < 8; i++) {
        oled_write_data(hi2c, F8x16[c * 16 + i + 8]);
    }
}

void oled_paint_string(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, char *str) {
    for(uint8_t i = 0; str[i] != '\0'; i++) {
        oled_paint_char(hi2c, x, y, str[i]);
        x += 8;
        if(x > 120) {
            x = 0;
            y += 2;
        }
    }
}

void oled_paint_hz(I2C_HandleTypeDef *hi2c, uint8_t x, uint8_t y, uint8_t hz_id) {
	const uint8_t *p = hz16x16[hz_id];
    oled_set_pos(hi2c, x, y);
    for(uint8_t i = 0; i < 16; i++) {
        oled_write_data(hi2c, p[i]);
    }
    oled_set_pos(hi2c, x, y + 1);
    for(uint8_t i = 16; i < 32; i++) {
        oled_write_data(hi2c, p[i]);
    }
}
