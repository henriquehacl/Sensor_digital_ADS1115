uint8_t HoldingReg(uint8_t fc, uint16_t address, uint16_t length)
{

    // Check if the requested addresses exist in the array
    if (address > Holding_register_size || (address + length) > Holding_register_size)
    {
        return STATUS_ILLEGAL_DATA_ADDRESS;
    }

    // Read the analog inputs
    for (int i = 0; i < length; i++)
    {
        // Write the state of the analog pin to the response buffer.
        slave.writeRegisterToBuffer(i, Holding_register[address + i]);
    }

    return STATUS_OK;
}

uint8_t writeMemory(uint8_t fc, uint16_t address, uint16_t length)
{
    // Write the received data to EEPROM.
    for (int i = 0; i < length; i++)
    {

        if(address + i == 0x00)
        {

            uint16_t value = slave.readRegisterFromBuffer(i);
            if(value < 1 || value > 247)
            {
              return STATUS_ILLEGAL_DATA_VALUE; 
            }
            if(ENDERECO_MODBUS != value)
            {
               ENDERECO_MODBUS = value;
               Grava_endereco_modbus(ENDERECO_MODBUS);
               slave.setUnitAddress(ENDERECO_MODBUS);
            }

            
        }

        
        if(address + i == 0x01)
        {
            uint16_t value = slave.readRegisterFromBuffer(i);
            if(value > 100)
            {
              return STATUS_ILLEGAL_DATA_VALUE; 
            }
            if(Temperature_offset != value)
            {
               Temperature_offset = value;
               Grava_Temperatura_offset(Temperature_offset);
            }
            
        }

        if(address + i == 0x02)
        {
            uint16_t value = slave.readRegisterFromBuffer(i);
            if(value > 100)
            {
              return STATUS_ILLEGAL_DATA_VALUE; 
            }
            if(umidade_offset != value)
            {
               umidade_offset = value;
               Grava_Umidade_offset(umidade_offset);
            }
            
            
        }

      }
    
    return STATUS_OK;
}

uint8_t InputReg(uint8_t fc, uint16_t address, uint16_t length)
{
    // Check if the requested addresses exist in the array
    if (address > Input_register_size || (address + length) > Input_register_size)
    {
        return STATUS_ILLEGAL_DATA_ADDRESS;
    }

    // Read the analog inputs
    for (int i = 0; i < length; i++)
    {
        // Write the state of the analog pin to the response buffer.
        slave.writeRegisterToBuffer(i, Input_register[address + i]);
    }

    return STATUS_OK;
}

void Load_holding_register()
{

  Holding_register[0] = ENDERECO_MODBUS; //ENDERECO MODBUS
  Holding_register[1] = Temperature_offset;
  Holding_register[2] = umidade_offset;
  
}


void Load_input_register()
{

  Input_register[0] = ID_EQUIPAMENTO;       
  Input_register[1] = VERSAO_FIRMWARE * 10;
  Input_register[2] = (uint16_t)(TEMPERATURA_AMBIENTE * 10);
  Input_register[3] = (uint16_t)(UMIDADE_AMBIENTE * 10);
  Input_register[4] = (uint16_t)((TEMPERATURA_AMBIENTE * 10) + Temperature_offset);
  Input_register[5] = (uint16_t)((UMIDADE_AMBIENTE * 10) + umidade_offset);
  Input_register[6] = TP_FALHA;
  Input_register[7] = UR_FALHA;

}