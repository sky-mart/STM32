#ifndef LSM303DLHC_H
#define LSM303DLHC_H

#include <stdint.h>

typedef enum {
	LSM303DLHC_CTRL_REG_A = 0x20,
} lsm303dlhc_acc_reg_t;

typedef enum {
    LSM303DLHC_CRA_REG_M = 0x00,
	LSM303DLHC_CRB_REG_M = 0x01,
	LSM303DLHC_MR_REG_M = 0x02,
	LSM303DLHC_OUT_X_H_M = 0x03,
	LSM303DLHC_OUT_X_L_M = 0x04,
	LSM303DLHC_OUT_Y_H_M = 0x05,
	LSM303DLHC_OUT_Y_L_M = 0x06,
	LSM303DLHC_OUT_Z_H_M = 0x07,
	LSM303DLHC_OUT_Z_L_M = 0x08,
	LSM303DLHC_SR_REG_M = 0x09,
	LSM303DLHC_IRA_REG_M = 0x0A,
	LSM303DLHC_IRB_REG_M = 0x0B,
	LSM303DLHC_IRC_REG_M = 0x0C,
	LSM303DLHC_TEMP_OUT_H_M = 0x31,
	LSM303DLHC_TEMP_OUT_H_L = 0x32
} lsm303dlhc_mag_reg_t;

void lsm303dlhc_init();

uint8_t lsm303dlhc_acc_reg_read(lsm303dlhc_acc_reg_t);
uint8_t lsm303dlhc_mag_reg_read(lsm303dlhc_mag_reg_t);

#endif // LSM303DLHC_H