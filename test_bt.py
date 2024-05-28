import bluetooth
import time

# ESP32的蓝牙MAC地址
ESP32_MAC = "A0:A3:B3:2B:EC:2A"

# 要发送的数据
data_to_send = "Hello, ESP32!"

def connect_and_send():
    # 创建蓝牙套接字
    sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

    # 连接到ESP32
    print(f"Connecting to ESP32 at {ESP32_MAC}...")
    sock.connect((ESP32_MAC, 1))
    print("Connected!")

    # 发送数据
    print(f"Sending data: {data_to_send}")
    sock.send(data_to_send.encode())

    # 关闭套接字
    sock.close()
    print("Disconnected.")

if __name__ == "__main__":
    while True:
        try:
            connect_and_send()
            break
        except bluetooth.btcommon.BluetoothError as e:
            print(f"Error: {e}")
            time.sleep(5)  # 等待5秒后重试