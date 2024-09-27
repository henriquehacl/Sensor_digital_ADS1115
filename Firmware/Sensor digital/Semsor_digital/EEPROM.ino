bool Verifica_EEPROM() {

  uint8_t dadoInicial = EEPROM.read(0x00);

  if (dadoInicial != 0x30) {
    return false;
  }

  return true;
}

void Grava_verificacao_EEPROM() {
  EEPROM.write(0x00, 0x30);
}

//GRAVA QUAL SENSOR VAI OPERAR ENDERECO 0x01
void Grava_endereco_modbus(uint8_t Value) {
  if (Value < 1 || Value > 247) {
    Value = 1;
  }

  EEPROM.write(0x01, Value);
}

uint8_t leitura_endereco_modbus() {

  uint8_t Value = EEPROM.read(0x01);

  if (Value < 1 || Value > 247) {
    Value = 1;
  }

  return Value;
}
//------------------------------------------

//GRAVA QUAL SENSOR VAI OPERAR ENDERECO 0x01
void Grava_Temperatura_offset(uint8_t Value) {
  if (Value > 100) {
    Value = 0;
  }

  EEPROM.write(0x02, Value);
}

uint8_t leitura_Temperatura_offset() {

  uint8_t Value = EEPROM.read(0x02);

  if (Value > 100) {
    Value = 0;
  }

  return Value;
}
//------------------------------------------

//GRAVA QUAL SENSOR VAI OPERAR ENDERECO 0x01
void Grava_Umidade_offset(uint8_t Value) {
  if (Value > 100) {
    Value = 0;
  }

  EEPROM.write(0x03, Value);
}

uint8_t leitura_Umidade_offset() {

  uint8_t Value = EEPROM.read(0x03);

  if (Value > 100) {
    Value = 0;
  }

  return Value;
}
//------------------------------------------