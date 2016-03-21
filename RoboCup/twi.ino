static void Wire_Init(void) {
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
}

//-------------------------------------------------------------------------------

static void Wire1_Init(void) {
  pmc_enable_periph_clk(WIRE1_INTERFACE_ID);
  PIO_Configure(
    g_APinDescription[PIN_WIRE1_SDA].pPort,
    g_APinDescription[PIN_WIRE1_SDA].ulPinType,
    g_APinDescription[PIN_WIRE1_SDA].ulPin,
    g_APinDescription[PIN_WIRE1_SDA].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PIN_WIRE1_SCL].pPort,
    g_APinDescription[PIN_WIRE1_SCL].ulPinType,
    g_APinDescription[PIN_WIRE1_SCL].ulPin,
    g_APinDescription[PIN_WIRE1_SCL].ulPinConfiguration);

  NVIC_DisableIRQ(TWI0_IRQn);
  NVIC_ClearPendingIRQ(TWI0_IRQn);
  NVIC_SetPriority(TWI0_IRQn, 0);
  NVIC_EnableIRQ(TWI0_IRQn);
}

//-------------------------------------------------------------------------------

uint8_t readByte() {
  TWI_WaitByteReceived(pTwi, RECV_TIMEOUT);
  while (!TWI_ByteReceived(pTwi))
    ;
  return TWI_ReadByte(pTwi);
}

//-------------------------------------------------------------------------------

static inline bool TWI_WaitTransferComplete(Twi *_twi, uint32_t _timeout) {
  while (!TWI_TransferComplete(_twi)) {
    if (TWI_FailedAcknowledge(_twi))
      return false;
    if (--_timeout == 0)
      return false;
  }
  return true;
}

//-------------------------------------------------------------------------------

static inline bool TWI_WaitByteReceived(Twi *_twi, uint32_t _timeout) {
  while (!TWI_ByteReceived(_twi)) {
    if (TWI_FailedAcknowledge(_twi))
      return false;
    if (--_timeout == 0)
      return false;
  }
  return true;
}

//-------------------------------------------------------------------------------

static inline bool TWI_FailedAcknowledge(Twi *pTwi) {
  return pTwi->TWI_SR & TWI_SR_NACK;
}
