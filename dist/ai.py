from openai import OpenAI
import os
from ai_config import *

# 初始化DeepSeek客户端
client = OpenAI(api_key=API_KEY, base_url=API_PLANTFORM)


def load_history():
    """从文件加载对话历史，chat_history.txt的读取采用逐行读取，换行符读取时有<br>与\n的转换"""
    tmp = "\n"
    if os.path.exists(HISTORY_FILE):
        with open(HISTORY_FILE, "r", encoding="utf-8") as f:
            return [
                line.strip().replace("<br>", tmp).split("|", 1)
                for line in f.readlines()
            ]
    return []


def save_history(messages):
    """保存对话历史到文件"""
    tmp = "\n"
    with open(HISTORY_FILE, "w", encoding="utf-8") as f:
        for msg in messages[-MAX_HISTORY:]:  # 只保存最近的MAX_HISTORY条记录
            f.write(f"{msg['role']}|{msg['content'].replace(tmp, '<br>')}\n")


def chat(string):
    # 消息历史
    history = load_history()
    messages = [{"role": role, "content": content} for role, content in history]
    messages.append({"role": "user", "content": string})

    # 调用DeepSeek API
    response = client.chat.completions.create(
        model="deepseek-chat",
        messages=[{"role": "system", "content": SYSTEM_PROMPT}] + messages,
        stream=False,
    )

    # 添加助手回复到消息列表
    messages.append(
        {
            "role": "assistant",
            "content": response.choices[0].message.content,
        }
    )

    # 保存对话历史
    save_history(messages)


# 添加命令行支持
if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        user_input = "".join(sys.argv[1:])
        response = chat(user_input)
