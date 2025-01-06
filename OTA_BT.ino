//C:\Users\cznaz\OneDrive\Empresa\BT_OTA\OTA_BT\build\esp32.esp32.esp32/OTA_BT.ino.bin



#include <BluetoothSerial.h>
#include <Update.h>
#include <Crescer.h>

Tempora temp1Bt;
Tempora temp2Bt;

int setaBt = 0;


BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_OTA_BT");

  Serial.println("ESP32 OTA via Bluetooth iniciado!");
  temp1Bt.defiSP(10000);
 
  temp2Bt.defiSP(100);
  pinMode(2, OUTPUT);
}





int conta = 0;

void loop() {
  if (SerialBT.available()) {
    handleOTA();
  }
    delay(10);  // Pequeno atraso para evitar travamentos

  if (temp2Bt.Saida(1)) {
    digitalWrite(2, 1 - digitalRead(2));
    temp2Bt.Saida(0);
    //Serial.println(conta++);
  }

  
  if (temp1Bt.Saida(setaBt)) {

    Serial.println("Timeout atingido. Tentando finalizar OTA...");
    if (Update.end(true)) {
      Serial.println("OTA concluída com sucesso!");
      Serial.println("Reiniciando...");
      SerialBT.print("Ota com Sucesso");

      ESP.restart();
    } else {
      Serial.println("Erro ao finalizar OTA!");
      Serial.printf("Erro: %s\n", Update.errorString());

      SerialBT.println("Erro ao finalizar OTA!");

      delay(2000);
      ESP.restart();
    }
  }
  
}








void handleOTA() {
  static size_t totalBytes = 0;
  static unsigned long lastReceiveTime = millis();
  const unsigned long timeout = 10000;  // Timeout de 20 segundos sem dados

  if (!Update.isRunning()) {
    Serial.println("Iniciando OTA via Bluetooth...");
    if (Update.begin(UPDATE_SIZE_UNKNOWN)) {
      Serial.println("Atualização iniciada...");

    } else {
      Serial.println("Falha ao iniciar atualização!");
      return;
    }
  }

  while (SerialBT.available()) {
    uint8_t buffer[64];  // Buffer de 1 KB
    size_t len = SerialBT.readBytes(buffer, sizeof(buffer));
    if (Update.write(buffer, len)) {
      totalBytes += len;
      lastReceiveTime = millis();
      Serial.printf("Bytes recebidos: %d\n", totalBytes);
    //   Serial.printf("status Update: %d\n", Update.end(true));

 //if (Update.end(true)) {
   


      if (len != 64) {
        Serial.println("Falta de bytes no recebimento");
        Serial.println("Reiniciando...");

        SerialBT.printf("Menos de 64 bytes recebidos, vai resetar o ESP32. Total recebidos: %d\n", totalBytes);

 if (Update.end(true)) {
      Serial.println("OTA concluída com sucesso!");
      Serial.println("Reiniciando...");
      SerialBT.println("Ota com Sucesso");
     
    } else {
      Serial.println("Erro ao finalizar OTA!");
      Serial.printf("Erro: %s\n", Update.errorString());
      SerialBT.println("Erro ao finalizar OTA!");
      
    }

        delay(2000);
        ESP.restart();
      }

      temp1Bt.Saida(0);
      setaBt = 1;
    }
  }
  //Serial.println("AQUI");
  // Diagnóstico: Verificar quando o timeout é atingido
  if (millis() - lastReceiveTime > timeout) {
    Serial.println("Timeout atingido. Tentando finalizar OTA...");
    if (Update.end(true)) {
      Serial.println("OTA concluída com sucesso!");
      Serial.println("Reiniciando...");
      ESP.restart();
    } else {
      Serial.println("Erro ao finalizar OTA!");
      Serial.printf("Erro: %s\n", Update.errorString());
      SerialBT.println("Erro ao finalizar OTA!");

      delay(2000);
      ESP.restart();
    }
  } else {
    Serial.printf("Ainda aguardando dados... Tempo desde o último dado: %lu ms\n", millis() - lastReceiveTime);
  }
}
