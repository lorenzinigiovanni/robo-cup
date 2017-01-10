/****************************************************************************** 
MLX90614.h
Montagni Massimiliano e Lorenzini Giovanni
******************************************************************************/

#ifndef MLX90164_H
#define MLX90164_H

#include <Arduino.h>
#include <include/twi.h>

//////////////////////////////////
// MLX90614 Default I2C Address //
//////////////////////////////////

#define _deviceAddress 0x5A

//EEPROM 32x16
#define TO_MAX    0x00
#define TO_MIN    0x01
#define PWM_CTRL  0x02

//RAM 32x16
#define RAW_IR_1  0x04
#define RAW_IR_2  0x05
#define TA        0x06
#define TOBJ_1    0x07
#define TOBJ_2    0x08


class IRTherm 
{
public:
	// Default constructor, does very little besides setting class variable
	// initial values.
	IRTherm();
	
	// begin(<address>) initializes the Wire library, and prepares 
	// communication with an MLX90614 device at the specified 7-bit I2C 
	// address.
	// If no parameter is supplied, the default MLX90614 address is used.
	uint8_t begin(uint8_t address = MLX90614_DEFAULT_ADDRESS);
    
	// read() pulls the latest ambient and object temperatures from the 
	// MLX90614. It will return either 1 on success or 0 on failure. (Failure 
	// can result from either a timed out I2C transmission, or an incorrect
	// checksum value).
    
    uint8_t read(void);
	
	// object() returns the MLX90614's most recently read object temperature 
	// after the read() function has returned successfully. The float value
	// returned will be in the units specified by setUnit().
	float object(void);
	
	// ambient() returns the MLX90614's most recently read ambient temperature 
	// after the read() function has returned successfully. The float value
	// returned will be in the units specified by setUnit().
	float ambient(void);
	
	// readEmissivity() reads the MLX90614's emissivity setting. It will 
	// return a value between 0.1 and 1.0.
	float readEmissivity(void);
	
	// setEmissivity(<emis>) can set the MLX90614's configured emissivity 
	// EEPROM value.
	// The <emis> parameter should be a value between 0.1 and 1.0.
	// The function will return either 1 on success or 0 on failure.
	uint8_t setEmissivity(float emis);
	
	// readAddress() returns the MLX90614's configured 7-bit I2C bus address.
	// A value between 0x01 and 0x7F should be returned.
	uint8_t readAddress(void);
	
	// setAddress(<newAdd>) can set the MLX90614's 7-bit I2C bus address.
	// The <newAdd> parameter should be a value between 0x01 and 0x7F.
	// The function returns 1 on success and 0 on failure.
	// The new address won't take effect until the device is reset.
	uint8_t setAddress(uint8_t newAdd);
	
	// readID() reads the 64-bit ID of the MLX90614.
	// Return value is either 1 on success or 0 on failure.
	uint8_t readID(void);
	
	// After calling readID() getIDH() and getIDL() can be called to read
	// the upper 4 bytes and lower 4-bytes, respectively, of the MLX90614's
	// identification registers.
	uint32_t getIDH(void);
	uint32_t getIDL(void);
	
	// readRange() pulls the object maximum and minimum values stored in the
	// MLX90614's EEPROM.
	// It will return either 1 on success or 0 on failure.
	uint8_t readRange(void);
	
	// minimum() and maximum() return the MLX90614's minimum and maximum object
	// sensor readings.
	// The float values returned will be in the units specified by setUnit().
	float minimum(void);
	float maximum(void);
	
	// setMax(<maxTemp>) and setMin(<minTemp>) configure the MLX90614's 
	// maximum and minimum object sensor temperatures.
	uint8_t setMax(float maxTemp);
	uint8_t setMin(float minTemp);
	
	// sleep() sets the MLX90614 into a low-power sleep mode.
	uint8_t sleep(void);
	
	// wake() should revive the MLX90614 from low-power sleep mode.
	uint8_t wake(void);
	
private:
    uint16_t id[4]; // Keeps track of the 64-bit ID value
	
    uint8_t readByte(void);
    
    static inline bool TWI_WaitTransferComplete(Twi *_twi, uint32_t _timeout);
    static inline bool TWI_WaitByteReceived(Twi *_twi, uint32_t _timeout);
    static inline bool TWI_FailedAcknowledge(Twi *pTwi);
	// Abstract function to write 16-bits to an address in the MLX90614's
	// EEPROM
	uint8_t writeEEPROM(byte reg, int16_t data);
	
	// Abstract functions to read and write 16-bit values from a RAM
	// or EEPROM address in the MLX90614
	uint8_t I2CReadWord(byte reg, int16_t * dest);
	uint8_t I2CWriteWord(byte reg, int16_t data);
	
	// crc8 returns a calculated crc value given an initial value and
	// input data.
	// It's configured to calculate the CRC using a x^8+x^2+x^1+1 poly
	uint8_t crc8 (uint8_t inCrc, uint8_t inData);
};

#endif