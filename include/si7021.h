/* This file is part of SI7021 Library for ESP-IDF framework.
 *
 *  SI7021 Library for ESP-IDF framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SI7021 Library for ESP-IDF framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SI7021 Library for ESP-IDF framework.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file si7021.h
 * @author Le Nguyen Hoang Nhan
 * @date 19 May 2019
 *
 * @brief SI7021 Library for ESP-IDF framework header file.
 *
 * @see https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf
 * @copyright Copyright (C) 2019  Le Nguyen Hoang Nhan. All rights reserved.
 */

#ifndef COMPONENTS_SI7021_INCLUDE_SI7021_H_
#define COMPONENTS_SI7021_INCLUDE_SI7021_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <stdlib.h>
#include <stdio.h>

#define SI7021_ADDR		0x40                    /*!< SI7021 default address */
/**
 * @defgroup SI7021_I2C_CMD SI7021 I2C Commands
 *
 * @{
 */

#define SI7021_MEASRH_HOLD_CMD           0xE5   /*!< I2C command for RH measuring, hold master mode */
#define SI7021_MEASRH_NOHOLD_CMD         0xF5	/*!< I2C command for RH measuring, no hold master mode */
#define SI7021_MEASTEMP_HOLD_CMD         0xE3   /*!< I2C command for Temperature measuring, hold master mode */
#define SI7021_MEASTEMP_NOHOLD_CMD       0xF3	/*!< I2C command for Temperature measuring, no hold master mode */
#define SI7021_READPREVTEMP_CMD          0xE0	/*!< I2C command for reading previously measured temperature */
#define SI7021_RESET_CMD                 0xFE	/*!< I2C command for reseting sensors */
#define SI7021_WRITERHT_REG_CMD          0xE6	/*!< I2C command for writing RH/T user register 1 */
#define SI7021_READRHT_REG_CMD           0xE7	/*!< I2C command for reading RH/T user register 1 */
#define SI7021_WRITEHEATER_REG_CMD       0x51	/*!< I2C command for writing heater register */
#define SI7021_READHEATER_REG_CMD        0x11	/*!< I2C command for reading heater register */
#define SI7021_ID1_CMD                   0xFA0F	/*!< I2C command for reading 1st byte of electronic ID */
#define SI7021_ID2_CMD                   0xFCC9	/*!< I2C command for reading 2nd byte of electronic ID */
#define SI7021_FIRMVERS_CMD              0x84B8 /*!< I2C command for reading firmware revision */
#define SI7021_SOFT_RESET_CMD            0xFE   /*!< I2C command for soft reseting sensors */
/**
 * @}
 *
 * @defgroup SI7021_RT_VALUE SI7021 Return Value
 *
 * @{
 */
// return values
#define SI7021_ERR_OK				0x00		/*!< Generic OK return value */
#define SI7021_ERR_CONFIG			0x01		/*!< Error configure i2c driver for sensor. @see ::__si7021_param_config() */
#define SI7021_ERR_INSTALL			0x02		/*!< Error install i2c driver for sensor. @see ::__si7021_driver_config() */
#define SI7021_ERR_NOTFOUND			0x03		/*!< Cannot find sensor. @see ::si7021_check_availability() */
#define SI7021_ERR_INVALID_ARG		0x04		/*!< Invalid argument, correlated to esp_err.h#ESP_ERR_INVALID_ARG */
#define SI7021_ERR_FAIL		 		0x05		/*!< Generic FAIL return value */
#define SI7021_ERR_INVALID_STATE	0x06		/*!< Sensor in a invalid state */
#define SI7021_ERR_TIMEOUT	 		0x07		/*!< Timed out communicating with sensor */

/**
 * @}
 *
 * @defgroup SI7021_RES_HEX SI7021 resolution in hex
 *
 * @{
 */
#define SI7021_12_14_RES			0x00		/*!< 12bit RH resolution, 14bit temperature resolution */
#define SI7021_8_12_RES				0x01		/*!<  8bit RH resolution, 12bit temperature resolution */
#define SI7021_10_13_RES			0x80		/*!< 10bit RH resolution, 13bit temperature resolution */
#define SI7021_11_11_RES			0x81		/*!< 11bit RH resolution, 11bit temperature resolution */
/**
 * @}
 */

#define SI7021_VDD_OK				0x01		/*!< VDD is OK (>1.9 V) */
#define SI7021_VDD_LOW				0x00		/*!< VDD is LOW (1.9~1.8V) */

#define SI7021_HEATER_ON			0x01		/*!< Heater is ON */
#define SI7021_HEATER_OFF			0x00		/*!< Heater is OFF */

/**
 * @brief SI7021 initialization parameter
 * @see #__si7021_config
 * @see i2c.h#i2c_config_t
 */
typedef struct si7021_config_t {

	i2c_config_t sensors_config; /*!< I2C configuration of driver of SI7021. */

	i2c_port_t si7021_port; /*!< I2C port of SI7021 sensors, default to port 0. */

} si7021_config_t;


/**
 * @brief Error type for return value.
 */
typedef uint8_t si7021_err_t;


/**
 * @brief Internal variable for storing sensor information.
 */
si7021_config_t __si7021_config;


/**
 * @brief Initialize SI7021 sensor
 * @param config #si7021_config_t struct that contain sensors information and configuration
 * @return
 * 		- #SI7021_ERR_OK Success.
 * 		- #SI7021_ERR_CONFIG Failed to configure I2C parameter, forwarded return from #__si7021_param_config(#si7021_config_t *config)
 * 		- #SI7021_ERR_INSTALL Failed to configure I2C Driver, forwarded return from #__si7021_driver_config(#si7021_config_t *config)
 * 		- #SI7021_ERR_NOTFOUND Sensor missing and/or not available, forwarded return from #si7021_check_availability()
 */
si7021_err_t si7021_init(si7021_config_t *config);


/**
 * @brief Configure I2C Parameter for SI7021
 * @note Internal use only
 * @param config Forwarded parameter from #si7021_init(#si7021_config_t *config)
 * @return
 * 		- #SI7021_ERR_OK Success.
 * 		- #SI7021_ERR_CONFIG Failed to configure I2C parameter
 */
si7021_err_t __si7021_param_config(si7021_config_t *config);


/**
 * @brief Configure I2C Driver for SI7021
 * @note Internal use only
 * @param config Forwarded parameter from #si7021_init(#si7021_config_t *config)
 * @return
 * 		- #SI7021_ERR_OK Success
 * 		- #SI7021_ERR_CONFIG Failed to install I2C parameters
 */
si7021_err_t __si7021_driver_config(si7021_config_t *config);


/**
 * @brief Get data from sensors by issuing read command and read data return by sensor
 * @note Internal use only
 * @param cmd Command that will be sent to sensor
 * @return 16bit value (uint16_t) contain data from sensors
 * @note This function will print to console if crc value is invalid
 */
uint16_t __si7021_read(uint8_t cmd);


/**
 * @brief Check data integrity with crc
 * @param value 16bit (uint16_t) value that contain data return by sensors.
 * @param crc 8bit (uint8_t) value of crc data
 * @return
 * 		- boolean value, true if crc value is valid, otherwise false
 */
bool __is_crc_valid(uint16_t value, uint8_t crc);


/**
 * @brief Check the availability of sensor
 * @return
 *		- #SI7021_ERR_OK Sensor detected and available
 *		- #SI7021_ERR_NOTFOUND Sensor missing and/or not available
 */
si7021_err_t si7021_check_availability();


/**
 * @brief Read temperature value from sensor
 * @return float value, temperature in <a href="https://en.wikipedia.org/wiki/Celsius">Celsius</a>
 */
float si7021_read_temperature();


/**
 * @brief Read <a href="https://en.wikipedia.org/wiki/Relative_humidity">Relative Humidity</a> value from sensor
 * @return float value, <a href="https://en.wikipedia.org/wiki/Relative_humidity">Relative Humidity</a> in percentage
 */
float si7021_read_humidity();


/**
 * @brief Read RH/T user register 1 from sensor
 * @note Internal use only
 * @return
 * 		- 8bit (unit8_t) value, contain content of user RH/T register
 * 		- 0 if failed to read
 */
uint8_t __si7021_read_user_register();


/**
 * @brief Write value to RH/T user register 1
 * @note Internal use only
 * @param value Value to write to register
 * @return
 * 		- #SI7021_ERR_OK Success
 * 		- #SI7021_ERR_INVALID_ARG Invalid arguments
 * 		- #SI7021_ERR_FAIL Failed to write
 * 		- #SI7021_ERR_INVALID_STATE Sensor is in invalid state
 * 		- #SI7021_ERR_TIMEOUT Timed out communicating with sensor
 */
si7021_err_t __si7021_write_user_register(uint8_t value);


/**
 * @brief Get current resolution of sensor
 * @return
 * 		- 0x00: 12bit RH resolution, 14bit temperature resolution
 * 		- 0x01: 8bit RH resolution, 12bit temperature resolution
 * 		- 0x80: 10bit RH resolution, 13bit temperature resolution
 * 		- 0x81: 11bit RH resolution, 11bit temperature resolution
 */
uint8_t si7021_get_resolution();

/**
 * @brief Reset the sensor
 * @note Reset will erase all setting, register...
 * @return
 * 		- #SI7021_ERR_OK Success
 * 		- #SI7021_ERR_TIMEOUT Timed out communicating with sensor
 * 		- #SI7021_ERR_INVALID_STATE Sensor is in invalid state
 * 		- #SI7021_ERR_FAIL Failed
 * 		- #SI7021_ERR_INVALID_ARG Invalid arguments
 */
si7021_err_t si7021_soft_reset();

/**
 * @brief Set the sensors resolution
 * @param resolution The resolution will be set
 * @return
 * 		- #SI7021_ERR_OK Success
 * 		- #SI7021_ERR_INVALID_ARG Invalid arguments
 * 		- #SI7021_ERR_FAIL Failed to write
 * 		- #SI7021_ERR_INVALID_STATE Sensor is in invalid state
 * 		- #SI7021_ERR_TIMEOUT Timed out communicating with sensor
 */
si7021_err_t si7021_set_resolution(uint8_t resolution);


/**
 * @brief Set heater register
 * @param value 8bit (uint8_t) value that will be written to heater register
 * 		- 0xF: 94.20mA
 * 		- 0x8: 51.96mA
 * 		- 0x4: 27.39mA
 * 		- 0x2: 15.24mA
 * 		- 0x1: 9.18mA
 * 		- 0x0: 3.09mA
 */
si7021_err_t si7021_set_heater_register(uint8_t value);

/**
 * @brief Set heater status
 * @param value Heater status will be set to this
 * 		- #SI7021_HEATER_ON: HEATER will be turn on
 * 		- #SI7021_HEATER_OFF: HEATER will be turn off
 */
si7021_err_t si7021_set_heater_status(uint8_t value);
/**
 * @brief Read heater register
 * @return 8bit (uint8_t) value contain content of heater register
 * 		- 0xF: 94.20mA
 * 		- 0x8: 51.96mA
 * 		- 0x4: 27.39mA
 * 		- 0x2: 15.24mA
 * 		- 0x1: 9.18mA
 * 		- 0x0: 3.09mA
 */
uint8_t si7021_get_heater_register();
/**
 * @brief Read sensor firmware revision
 * @return 8bit (uint8_t) value contain sensor firmware revision
 * 		- 0xFF: Firmware version 1.0
 * 		- 0x20: Firmware version 2.0
 */
uint8_t si7021_read_firmware_rev();

/**
 * @brief Check sensors VDD
 * @return 8bit (uint8_t) value
 * 		- #SI7021_VDD_OK VDD is OK
 * 		- #SI7021_VDD_LOW VDD is LOW
 * @note Consider changing power supply when VDD is LOW, sensors may not work correctly
 */
uint8_t si7021_read_vdd_status();
/**
 * @brief Check sensors heater status
 * @return 8bit (uint8_t) value
 * 		- #SI7021_HEATER_ON HEATER is ON
 * 		- #SI7021_HEATER_OFF HEATER is OFF
 */
uint8_t si7021_get_heater_status();

/**
 * @brief Get sensor electronic id
 * @return 64bit (uint64_t) value contain sensor electronic id
 */
uint64_t get_electronic_id();

#ifdef __cplusplus
}
#endif
#endif /* COMPONENTS_SI7021_INCLUDE_SI7021_H_ */
