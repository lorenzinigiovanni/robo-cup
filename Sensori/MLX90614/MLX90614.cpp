/******************************************************************************
 MLX90614.cpp
 Montagni Massimiliano e Lorenzini Giovanni
******************************************************************************/

#include <MLX90614.h>

static const uint32_t TWI_CLOCK = 100000;
static const uint32_t RECV_TIMEOUT = 100000;
static const uint32_t XMIT_TIMEOUT = 100000;

Twi *pTwi = WIRE_INTERFACE;

IRTherm::IRTherm()
{
	// Set initial values for all private member variables
    
}

uint8_t IRTherm::begin(uint8_t address)
{
    pmc_enable_periph_clk(WIRE_INTERFACE_ID);
    PIO_Configure(
                  g_APinDescription[PIN_WIRE_SDA].pPort,
                  g_APinDescription[PIN_WIRE_SDA].ulPinType,
                  g_APinDescription[PIN_WIRE_SDA].ulPin,
                  g_APinDescription[PIN_WIRE_SDA].ulPinConfiguration);
    PIO_Configure(
                  g_APinDescription[PIN_WIRE_SCL].pPort,
                  g_APinDescription[PIN_WIRE_SCL].ulPinType,
                  g_APinDescription[PIN_WIRE_SCL].ulPin,
                  g_APinDescription[PIN_WIRE_SCL].ulPinConfiguration);
    
    NVIC_DisableIRQ(TWI1_IRQn);
    NVIC_ClearPendingIRQ(TWI1_IRQn);
    NVIC_SetPriority(TWI1_IRQn, 0);
    NVIC_EnableIRQ(TWI1_IRQn);
    
    pTwi->TWI_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
    TWI_ConfigureMaster(pTwi, TWI_CLOCK, VARIANT_MCK);

	return 1; // Return success
}

uint8_t IRTherm::read()
{
	// read both the object and ambient temperature values
	if (readObject() && readAmbient())
	{
	// If the reads succeeded, return success
	return 1;
	}
	return 0; // Else return fail
}

uint8_t IRTherm::readByte() {
    //TWI_WaitByteReceived(pTwi, RECV_TIMEOUT);
    while (!TWI_ByteReceived(pTwi))
        ;
    return TWI_ReadByte(pTwi);
}

static inline bool IRTherm::TWI_WaitTransferComplete(Twi *_twi, uint32_t _timeout) {
    while (!TWI_TransferComplete(_twi)) {
        if (TWI_FailedAcknowledge(_twi))
            return false;
        if (--_timeout == 0)
            return false;
    }
    return true;
}

static inline bool IRTherm::TWI_WaitByteReceived(Twi *_twi, uint32_t _timeout) {
    while (!TWI_ByteReceived(_twi)) {
        if (TWI_FailedAcknowledge(_twi))
            return false;
        if (--_timeout == 0)
            return false;
    }
    return true;
}

static inline bool IRTherm::TWI_FailedAcknowledge(Twi *pTwi) {
    return pTwi->TWI_SR & TWI_SR_NACK;
}

uint8_t IRTherm::readRange()
{
	// Read both minimum and maximum values from EEPROM
	if (readMin() && readMax())
	{
	// If the read succeeded, return success
	return 1;
	}
	return 0; // Else return fail
}

float IRTherm::ambient(void)
{
	// Return the calculated ambient temperature
	return calcTemperature(_rawAmbient);
}

float IRTherm::object(void)
{
	// Return the calculated object temperature
	return calcTemperature(_rawObject);
}

float IRTherm::minimum(void)
{
	// Return the calculated minimum temperature
	return calcTemperature(_rawMin);
}

float IRTherm::maximum(void)
{
	// Return the calculated maximum temperature
	return calcTemperature(_rawMax);
}

uint8_t IRTherm::readObject()
{
	int16_t rawObj;
	// Read from the TOBJ1 register, store into the rawObj variable
	if (I2CReadWord(MLX90614_REGISTER_TOBJ1, &rawObj))
	{
	// If the read succeeded
	if (rawObj & 0x8000) // If there was a flag error
	{
	return 0; // Return fail
	}
	// Store the object temperature into the class variable
	_rawObject = rawObj;
	return 1;
	}
	return 0;	
}

uint8_t IRTherm::readObject2()
{
	int16_t rawObj;
	// Read from the TOBJ2 register, store into the rawObj variable
	if (I2CReadWord(MLX90614_REGISTER_TOBJ2, &rawObj))
	{
	// If the read succeeded
	if (rawObj & 0x8000) // If there was a flag error
	{
	return 0; // Return fail
	}
	// Store the object2 temperature into the class variable
	_rawObject2 = rawObj;
	return 1;
	}
	return 0;	
}

uint8_t IRTherm::readAmbient()
{
	int16_t rawAmb;
	// Read from the TA register, store value in rawAmb
	if (I2CReadWord(MLX90614_REGISTER_TA, &rawAmb))
	{
	// If the read succeeds, store the read value
	_rawAmbient = rawAmb; // return success
	return 1;
	}
	return 0; // else return fail
}

uint8_t IRTherm::readMax()
{
	int16_t toMax;
	// Read from the TOMax EEPROM address, store value in toMax
	if (I2CReadWord(MLX90614_REGISTER_TOMAX, &toMax))
	{
	_rawMax = toMax;
	return 1;
	}
	return 0;
}

uint8_t IRTherm::readMin()
{
	int16_t toMin;
	// Read from the TOMin EEPROM address, store value in toMax
	if (I2CReadWord(MLX90614_REGISTER_TOMIN, &toMin))
	{
	_rawMin = toMin;
	return 1;
	}
	return 0;
}

uint8_t IRTherm::setMax(float maxTemp)
{
	// Convert the unit-ed value to a raw ADC value:
	int16_t rawMax = calcRawTemp(maxTemp);
	// Write that value to the TOMAX EEPROM address:
	return writeEEPROM(MLX90614_REGISTER_TOMAX, rawMax);
}

uint8_t IRTherm::setMin(float minTemp)
{
	// Convert the unit-ed value to a raw ADC value:
	int16_t rawMin = calcRawTemp(minTemp);
	// Write that value to the TOMIN EEPROM address:	
	return writeEEPROM(MLX90614_REGISTER_TOMIN, rawMin);
}

uint8_t IRTherm::setEmissivity(float emis)
{
	// Make sure emissivity is between 0.1 and 1.0
	if ((emis > 1.0) || (emis < 0.1))
	return 0; // Return fail if not
	// Calculate the raw 16-bit value:
	uint16_t ke = uint16_t(65535.0 * emis);
	ke = constrain(ke, 0x2000, 0xFFFF);

	// Write that value to the ke register
	return writeEEPROM(MLX90614_REGISTER_KE, (int16_t)ke);
}

float IRTherm::readEmissivity(void)
{
	int16_t ke;
	if (I2CReadWord(MLX90614_REGISTER_KE, &ke))
	{
	// If we successfully read from the ke register
	// calculate the emissivity between 0.1 and 1.0:
	return (((float)((uint16_t)ke)) / 65535.0);
	}
	return 0; // Else return fail
}

uint8_t IRTherm::readAddress(void)
{
	int16_t tempAdd;
	// Read from the 7-bit I2C address EEPROM storage address:
	if (I2CReadWord(MLX90614_REGISTER_ADDRESS, &tempAdd))
	{
	// If read succeeded, return the address:
	return (uint8_t) tempAdd;
	}
	return 0; // Else return fail
}

uint8_t IRTherm::setAddress(uint8_t newAdd)
{
	int16_t tempAdd;
	// Make sure the address is within the proper range:
	if ((newAdd >= 0x80) || (newAdd == 0x00))
	return 0; // Return fail if out of range
	// Read from the I2C address address first:
	if (I2CReadWord(MLX90614_REGISTER_ADDRESS, &tempAdd))
	{
	tempAdd &= 0xFF00; // Mask out the address (MSB is junk?)
	tempAdd |= newAdd; // Add the new address
	
	// Write the new addres back to EEPROM:
	return writeEEPROM(MLX90614_REGISTER_ADDRESS, tempAdd);
	}	
	return 0;
}

uint8_t IRTherm::readID(void)
{	
	for (int i=0; i<4; i++)
	{
	int16_t temp = 0;
	// Read from all four ID registers, beginning at the first:
	if (!I2CReadWord(MLX90614_REGISTER_ID0 + i, &temp))
	return 0;
	// If the read succeeded, store the ID into the id array:
	id[i] = (uint16_t)temp;
	}
	return 1;
}

uint32_t IRTherm::getIDH(void)
{
	// Return the upper 32 bits of the ID
	return ((uint32_t)id[3] << 16) | id[2];
}

uint32_t IRTherm::getIDL(void)
{
	// Return the lower 32 bits of the ID
	return ((uint32_t)id[1] << 16) | id[0];
}

uint8_t IRTherm::sleep(void)
{
	// Calculate a crc8 value.
	// Bits sent: _deviceAddress (shifted left 1) + 0xFF
	uint8_t crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, MLX90614_REGISTER_SLEEP);
	
	// Manually send the sleep command:
	Wire.beginTransmission(_deviceAddress);
	Wire.write(MLX90614_REGISTER_SLEEP);
	Wire.write(crc);
	Wire.endTransmission(true);
	
	// Set the SCL pin LOW, and SDA pin HIGH (should be pulled up)
	/*pinMode(SCL, OUTPUT);
	digitalWrite(SCL, LOW);
	pinMode(SDA, INPUT);*/
}

uint8_t IRTherm::wake(void)
{
	// Wake operation from datasheet. (Doesn't seem to be working.)
	/*pinMode(SCL, INPUT); // SCL high
	pinMode(SDA, OUTPUT);
	digitalWrite(SDA, LOW); // SDA low*/
	delay(50); // delay at least 33ms
	//pinMode(SDA, INPUT); // SDA high
	delay(250);
	// PWM to SMBus mode:
	/*pinMode(SCL, OUTPUT);
	digitalWrite(SCL, LOW); // SCL low*/
	delay(10); // Delay at least 1.44ms
	//pinMode(SCL, INPUT); // SCL high
	Wire.begin();
}

int16_t IRTherm::calcRawTemp(float calcTemp)
{
	int16_t rawTemp; // Value to eventually be returned
	
	if (_defaultUnit == TEMP_RAW)
	{
	// If unit is set to raw, just return that:
	rawTemp = (int16_t) calcTemp;
	}
	else
	{
	float tempFloat;
	// First convert each temperature to Kelvin:
	if (_defaultUnit == TEMP_F)
	{
	// Convert from farenheit to Kelvin
	tempFloat = (calcTemp - 32.0) * 5.0 / 9.0 + 273.15;
	}
	else if (_defaultUnit == TEMP_C)
	{
	tempFloat = calcTemp + 273.15;
	}
	else if (_defaultUnit == TEMP_K)
	{
	tempFloat = calcTemp;
	}
	// Then multiply by 0.02 degK / bit
	tempFloat *= 50;
	rawTemp = (int16_t) tempFloat;
	}
	return rawTemp;
}

float IRTherm::calcTemperature(int16_t rawTemp)
{
	float retTemp;
	
	if (_defaultUnit == TEMP_RAW)
	{
	retTemp = (float) rawTemp;
	}
	else
	{
	retTemp = float(rawTemp) * 0.02;
	if (_defaultUnit != TEMP_K)
	{
	retTemp -= 273.15;
	if (_defaultUnit == TEMP_F)
	{
	retTemp = retTemp * 9.0 / 5.0 + 32;
	}
	}
	}
	
	return retTemp;
}

uint8_t IRTherm::I2CReadWord(byte reg, int16_t * dest)
{
	int timeout = I2C_READ_TIMEOUT;
	
	Wire.beginTransmission(_deviceAddress);
	Wire.write(reg);
	
	Wire.endTransmission(false); // Send restart
	Wire.requestFrom(_deviceAddress, (uint8_t) 3);
	
	while ((Wire.available() < 3) && (timeout-- > 0))
	delay(1);
	if (timeout <= 0)
	return 0;
	
	uint8_t lsb = Wire.read();
	uint8_t msb = Wire.read();
	uint8_t pec = Wire.read();
	
	uint8_t crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, (_deviceAddress << 1) + 1);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	
	if (crc == pec)
	{
	*dest = (msb << 8) | lsb;
	return 1;
	}
	else
	{
	return 0;
	}
}

uint8_t IRTherm::writeEEPROM(byte reg, int16_t data)
{	
	// Clear out EEPROM first:
	if (I2CWriteWord(reg, 0) != 0)
	return 0; // If the write failed, return 0
	delay(5); // Delay tErase
	
	uint8_t i2cRet = I2CWriteWord(reg, data);
	delay(5); // Delay tWrite
	
	if (i2cRet == 0)
	return 1;
	else
	return 0;	
}

uint8_t IRTherm::I2CWriteWord(byte reg, int16_t data)
{
	uint8_t crc;
	uint8_t lsb = data & 0x00FF;
	uint8_t msb = (data >> 8);
	
	crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	
	Wire.beginTransmission(_deviceAddress);
	Wire.write(reg);
	Wire.write(lsb);
	Wire.write(msb);
	Wire.write(crc);
	return Wire.endTransmission(true);
}

uint8_t IRTherm::crc8 (uint8_t inCrc, uint8_t inData)
{
	uint8_t i;
	uint8_t data;
	data = inCrc ^ inData;
	for ( i = 0; i < 8; i++ )
	{
	if (( data & 0x80 ) != 0 )
	{
	data <<= 1;
	data ^= 0x07;
	}
	else
	{
	data <<= 1;
	}
	}
	return data;
}