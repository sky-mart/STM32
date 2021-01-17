#ifndef LSM303DLHC_H
#define LSM303DLHC_H

#include <stdint.h>

typedef enum {
	LSM303DLHC_CTRL_REG1_A = 0x20,
	LSM303DLHC_CTRL_REG2_A = 0x21,
	LSM303DLHC_CTRL_REG3_A = 0x22,
	LSM303DLHC_CTRL_REG4_A = 0x23,
	LSM303DLHC_CTRL_REG5_A = 0x24,
	LSM303DLHC_CTRL_REG6_A = 0x25,
	LSM303DLHC_REFERENCE_A = 0x26,
	LSM303DLHC_STATUS_REG_A = 0x27,
	LSM303DLHC_OUT_X_L_A = 0x28,
	LSM303DLHC_OUT_X_H_A = 0x29,
	LSM303DLHC_OUT_Y_L_A = 0x2A,
	LSM303DLHC_OUT_Y_H_A = 0x2B,
	LSM303DLHC_OUT_Z_L_A = 0x2C,
	LSM303DLHC_OUT_Z_H_A = 0x2D,
	LSM303DLHC_FIFO_CTRL_REG_A = 0x2E,
	LSM303DLHC_FIFO_SRC_REG_A = 0x2F,
	LSM303DLHC_INT1_CFG_A = 0x30,
	LSM303DLHC_INT1_SOURCE_A = 0x31,
	LSM303DLHC_INT1_THS_A = 0x32,
	LSM303DLHC_INT1_DURATION_A = 0x33,
	LSM303DLHC_INT2_CFG_A = 0x34,
	LSM303DLHC_INT2_SOURCE_A = 0x35,
	LSM303DLHC_INT2_THS_A = 0x36,
	LSM303DLHC_INT2_DURATION_A = 0x37,
	LSM303DLHC_CLICK_CFG_A = 0x38,
	LSM303DLHC_CLICK_SRC_A = 0x39,
	LSM303DLHC_CLICK_THS_A = 0x3A,
	LSM303DLHC_TIME_LIMIT_A = 0x3B,
	LSM303DLHC_TIME_LATENCY_A = 0x3C,
	LSM303DLHC_TIME_WINDOW_A = 0x3D,
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

uint8_t lsm303dlhc_acc_reg_read_sync(lsm303dlhc_acc_reg_t reg);
uint8_t lsm303dlhc_mag_reg_read_sync(lsm303dlhc_mag_reg_t reg);
void lsm303dlhc_acc_regs_read_sync(lsm303dlhc_acc_reg_t reg, uint8_t* data, uint8_t size);
void lsm303dlhc_mag_regs_read_sync(lsm303dlhc_mag_reg_t reg, uint8_t* data, uint8_t size);
void lsm303dlhc_acc_regs_read_async(lsm303dlhc_acc_reg_t reg, uint8_t* data, uint8_t size);
void lsm303dlhc_mag_regs_read_async(lsm303dlhc_mag_reg_t reg, uint8_t* data, uint8_t size);
void lsm303dlhc_read_async_wait_to_finish();

void lsm303dlhc_acc_reg_write_sync(lsm303dlhc_acc_reg_t reg, uint8_t value);
void lsm303dlhc_mag_reg_write_sync(lsm303dlhc_mag_reg_t reg, uint8_t value);

#endif // LSM303DLHC_H