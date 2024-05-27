from zhipuai import ZhipuAI
client = ZhipuAI(api_key="3792c983804afc3fd88c6c3e353eda13.y7lNNLQxZMq3oUXk") # 填写您自己的APIKey
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
response = client.chat.completions.create(
    model="glm-4",  # 填写需要调用的模型名称
    messages=[
        {"role": "user", "content": "作为一名营销专家，请为智谱开放平台创作一个吸引人的slogan"},
    ],
)

print(response.choices[0].message)

# for trunk in response:
#     print(trunk)
    