'''3792c983804afc3fd88c6c3e353eda13.y7lNNLQxZMq3oUXk'''
from zhipuai import ZhipuAI
# import bluetooth
import time

messages = []

# ESP32的蓝牙MAC地址
ESP32_MAC = "00:11:22:33:44:55"

# # 蓝牙链接和传输信息的函数
# def connect_and_send(message):
#     # 创建蓝牙套接字
#     sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

#     print(f"Connecting to ESP32 at {ESP32_MAC}...")
#     sock.connect((ESP32_MAC, 1))
#     print("Connected!")

#     # 发送数据
#     print(f"Sending data: {message}")
#     sock.send(message.encode())

#     # 关闭套接字
#     sock.close()
#     print("Disconnected.")

def client_init(text):
    client = ZhipuAI(api_key="3792c983804afc3fd88c6c3e353eda13.y7lNNLQxZMq3oUXk")
    messages.append({"role": "system", "content": "你是一个英文翻译工作者，你需要把接下来收到的中文翻译成英文，一句话即可"})
    messages.append({"role": "system", "content": "接下来的工作是，你需要把翻译好的英文，转换成美式发音音标，单个音标用'/'分隔，单词与单词之间用_分隔"})
    # messages.append({"role": "system", "content": "例如：How are you?，你需要输出：/h//a//u/ /a://r/ /j//e/"})
    # messages.append({"role": "system", "content": "把接下来收到的中文翻译成英文并以美式发音的音标形式输出，单个音标之间用/分隔，单词之间用空格分隔，例如：输入：你好吗？输出：/h//a//u/ /a://r/ /j//e/"})
    messages.append({"role": "user", "content": "你好"})
    response = client.chat.completions.create(
        model="glm-4",  # glm4是百度研发的国产LLM
        messages=messages,
        # tools=tools,
    )
    print(response.choices[0].message)
    messages.append(response.choices[0].message.model_dump()) 

if __name__ == "__main__":

    while True:
        try:
            client_init()
            connect_and_send()
            break
        except bluetooth.btcommon.BluetoothError as e:
            print(f"Error: {e}")
            time.sleep(5)  # 等待5秒后重试