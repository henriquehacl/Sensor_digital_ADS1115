#include <ModbusSlave.h>
#include <EEPROM.h>
#include <avr/wdt.h>


#define ID_EQUIPAMENTO 0x04
#define VERSAO_FIRMWARE 1.0

#define SLAVE_ID        1      // The Modbus slave ID, change to the ID you want to use.
#define SERIAL_BAUDRATE 19200  // Change to the baudrate you want to use for Modbus communication.
#define SERIAL_PORT     Serial // Serial port to use for RS485 communication, change to the port you're using.
#define RS485_CTRL_PIN  4      // Change to the pin the RE/DE pin of the RS485 controller is connected to.

#define SENSOR_UMIDADE_PIN     A0
#define SENSOR_TEMPERATURA_PIN A1

Modbus slave(SERIAL_PORT, SLAVE_ID, RS485_CTRL_PIN);


/*Registradores MODBUS*/
uint16_t Holding_register[20];
uint16_t  Holding_register_size = sizeof(Holding_register) / sizeof(Holding_register[0]);
uint16_t  Input_register[20];
uint16_t Input_register_size = sizeof(Input_register) / sizeof(Input_register[0]);

bool TP_FALHA = false;
bool UR_FALHA = false;
double   TEMPERATURA_AMBIENTE = 25;
double   UMIDADE_AMBIENTE = 0;
uint8_t  Temperature_offset = 0;
uint8_t  umidade_offset = 0;

uint8_t ENDERECO_MODBUS = 1;

void setup() {
  // put your setup code here, to run once:

  wdt_enable(WDTO_4S);
  SERIAL_PORT.begin(SERIAL_BAUDRATE);
  slave.begin(SERIAL_BAUDRATE); 
  slave.cbVector[CB_READ_HOLDING_REGISTERS] = HoldingReg;
  slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = writeMemory;
  slave.cbVector[CB_READ_INPUT_REGISTERS] = InputReg;

  pinMode(SENSOR_UMIDADE_PIN, INPUT);
  pinMode(SENSOR_TEMPERATURA_PIN, INPUT);

  Config_init();


}

void loop() {
  // put your main code here, to run repeatedly:
  wdt_reset();
  slave.poll();
  Load_holding_register();
  Load_input_register();
  Read_TP();
  Read_UR();
  SoftReset();

}

void Read_UR()
{

  static uint8_t Cont_amostras = 0;
  static double Amostras = 0;
  static unsigned long TIMER_REFRESH = 0;
  double val = 0;
  double RH = 0;
  double Amostra_Temp = 0;
  static uint8_t CONT_FALHAS = 0;

  if((TIMER_REFRESH + 10) < millis())
  {

    Amostra_Temp = analogRead(SENSOR_UMIDADE_PIN);
    if((Amostra_Temp > 100) && (Amostra_Temp < 570)){

      Amostras += Amostra_Temp;
      Cont_amostras++;
      CONT_FALHAS = 0;

    }else{
      CONT_FALHAS++;
      if(CONT_FALHAS > 100)
      {
        UR_FALHA = true;
      }
    }

    if(Cont_amostras >= 100)
    {

      UR_FALHA = false;
      Amostras /= 100.0;
      val = Amostras;    
      val = (val * 5.0) / 1023.0;
      val = (val-0.5) / 0.02;
      if(TP_FALHA){
        UMIDADE_AMBIENTE = val/(1.0546 - (0.00216*25.0)); 
      }else{
        UMIDADE_AMBIENTE = val/(1.0546 - (0.00216*TEMPERATURA_AMBIENTE)); 
      }
      
      Cont_amostras = 0;
      Amostras = 0;

    }

    TIMER_REFRESH = millis(); 

  }
}

void Read_TP()
{
  static unsigned long TIMER_REFRESH = 0;
  static uint8_t Cont_amostras = 0;
  static double Amostras = 0;
  double Amostra_Temp = 0;
  static uint8_t CONT_FALHAS = 0;

  if((TIMER_REFRESH + 10) < millis())
  {
    
    Amostra_Temp = analogRead(SENSOR_TEMPERATURA_PIN);
    if(Amostra_Temp < 310)
    {

      Amostras += Amostra_Temp;
      Cont_amostras++;
      CONT_FALHAS = 0;

    }else{
      CONT_FALHAS++;
      if(CONT_FALHAS > 100)
      {
        TP_FALHA = true;
      }
    }

    if(Cont_amostras >= 100)
    {

      TP_FALHA = false;
      Amostras /= 100.0;
      TEMPERATURA_AMBIENTE = (Amostras*5.0/(1023.0))/0.01;
      Cont_amostras = 0;
      Amostras = 0;

    }
    TIMER_REFRESH = millis();
  }

}

void Config_init()
{
  if(Verifica_EEPROM())
  {
    ENDERECO_MODBUS = leitura_endereco_modbus();
    Temperature_offset = leitura_Temperatura_offset();
    umidade_offset = leitura_Umidade_offset();
    slave.setUnitAddress(ENDERECO_MODBUS);
  }else{
    ENDERECO_MODBUS = 1;
    Temperature_offset = 0;
    umidade_offset = 0;
    Grava_verificacao_EEPROM();
    Grava_endereco_modbus(ENDERECO_MODBUS);
    Grava_Temperatura_offset(Temperature_offset);
    Grava_Umidade_offset(umidade_offset);
    slave.setUnitAddress(ENDERECO_MODBUS);
  }
}

void SoftReset()
{
  if(millis() > 0xf72e32c0)
  {
    while(true);
  }
}
