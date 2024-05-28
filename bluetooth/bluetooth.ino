#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32-BT"); //蓝牙设备名称
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (SerialBT.available()) {
    // 读取从Python脚本发送的数据
    String received = SerialBT.readString();
    Serial.print("Received: ");
    Serial.println(received);

//    // 向Python脚本发送数据
//    SerialBT.write("Hello from ESP32!");
  }
  delay(20);
}
