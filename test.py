'''3792c983804afc3fd88c6c3e353eda13.y7lNNLQxZMq3oUXk'''
from zhipuai import ZhipuAI
client = ZhipuAI(api_key="3792c983804afc3fd88c6c3e353eda13.y7lNNLQxZMq3oUXk")
messages = []
# tools = [
#     {
#             "type": "function",
#             "function": {
#                 "name": "convert_CH2notes",
#                 "description": "把中文翻译成英文，仅输出对应的音标",
#                 "parameters": {
#                     "type": "object",
#                     "properties": {
#                         "original text": {
#                             "description": "中文",
#                             "type": "string"
#                         },
#                     },
#                     "required": [ "original text"]
#                 },
#             }
#         },
# ]
messages = []
messages.append({"role": "system", "content": "你是一个英文翻译工作者，你需要把接下来收到的中文翻译成英文，一句话即可"})
messages.append({"role": "system", "content": "接下来的工作是，你需要把翻译好的英文，转换成美式发音音标，单个音标用'/'分隔，单词与单词之间用_分隔"})
# messages.append({"role": "system", "content": "例如：How are you?，你需要输出：/h//a//u/ /a://r/ /j//e/"})
# messages.append({"role": "system", "content": "把接下来收到的中文翻译成英文并以美式发音的音标形式输出，单个音标之间用/分隔，单词之间用空格分隔，例如：输入：你好吗？输出：/h//a//u/ /a://r/ /j//e/"})
messages.append({"role": "user", "content": "你好吗？"})
response = client.chat.completions.create(
    model="glm-4",  # 填写需要调用的模型名称
    messages=messages,
    # tools=tools,
)
print(response.choices[0].message)
messages.append(response.choices[0].message.model_dump()) 