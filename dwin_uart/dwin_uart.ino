static unsigned long previous = 0;
const long interval = 2000;

typedef struct {
  uint8_t Rx_Index;
  uint8_t Rx_Data;
  uint8_t Transfer_Cplt;
  uint8_t Rx_Buffer[500];
  uint8_t Tx_Buffer[500];
  uint8_t Lenght;
  uint8_t Page;
} Connection_t;
Connection_t DWIN = { 0 };

typedef enum {
  ADR_0 = 0,
  ADR_TEST_VALUE_1 = 0x28,
  ADR_TEST_VALUE_2 = 0x04,
  ADR_TEST_VALUE_3 = 0x08,
  ADR_TEST_VALUE_4 = 0x12
} DWIN_ADR_t;
// DWIN_ADR_t dwin_L_adr;

void setup() {
  Serial.begin(115200);
}

void loop() {
  DWIN_receive();
  unsigned long current_time = millis();
  if (current_time - previous >= interval) {
    DWINSendTemp(ADR_TEST_VALUE_1, 20.42);
    DWINSendTemp(ADR_TEST_VALUE_2, 20.42);
    DWINSendTemp(ADR_TEST_VALUE_3, 20.42);
    DWINSendTemp(ADR_TEST_VALUE_4, 20.42);
    previous = current_time;
  }
}

void DWINSendTemp(uint8_t address, float temp) {
  uint16_t val = temp * 10;
  DWIN.Tx_Buffer[0] = 0x5A;
  DWIN.Tx_Buffer[1] = 0xA5;
  DWIN.Tx_Buffer[2] = 0x05;
  DWIN.Tx_Buffer[3] = 0x82;
  DWIN.Tx_Buffer[4] = 0x10;
  DWIN.Tx_Buffer[5] = (address)&0xFF;
  DWIN.Tx_Buffer[6] = (val >> 8) & 0xFF;
  DWIN.Tx_Buffer[7] = (val)&0xFF;

  // Transmit the data over Serial
  Serial.write(DWIN.Tx_Buffer, 8);
  // for (int i = 0; i <= 7; i++) {
  //   Serial.println(DWIN.Tx_Buffer[i],HEX);
  // }
}

void DWIN_receive(void) {
  if (Serial.available() > 0) {
    DWIN.Rx_Data = Serial.read();
    Serial.println(DWIN.Rx_Data, HEX);
    if (DWIN.Rx_Index == 0 && DWIN.Rx_Data == 0x5A) {
      DWIN.Rx_Buffer[DWIN.Rx_Index] = DWIN.Rx_Data;
      DWIN.Rx_Index++;
    } else if (DWIN.Rx_Index == 1 && DWIN.Rx_Data == 0xA5) {
      DWIN.Rx_Buffer[DWIN.Rx_Index] = DWIN.Rx_Data;
      DWIN.Rx_Index++;
    }

    else if (DWIN.Rx_Index == 2) {
      DWIN.Lenght = DWIN.Rx_Data;
      DWIN.Rx_Buffer[DWIN.Rx_Index] = DWIN.Rx_Data;
      DWIN.Rx_Index++;
    } else if (DWIN.Rx_Index >= 3 && DWIN.Rx_Index <= (DWIN.Lenght + 2)) {
      DWIN.Rx_Buffer[DWIN.Rx_Index] = DWIN.Rx_Data;
      DWIN.Rx_Index++;

      if (DWIN.Rx_Index == (DWIN.Lenght + 3)) {
        DWIN.Transfer_Cplt = 1;
        DWIN.Rx_Index = 0;
        Serial.print("Transfer_Cplt : ");
        Serial.println(DWIN.Transfer_Cplt);
      }
    }
  }
}