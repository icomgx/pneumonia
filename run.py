import requests
import json
from flask import Flask, request

app = Flask(__name__)

url = 'https://view.inews.qq.com/g2/getOnsInfo?name=disease_h5'
response = requests.get(url)
r_text = response.text


# 获取第一组数据 现有
def text1json(text):
    j1son = json.loads(json.loads(text)["data"])["chinaTotal"]
    return j1son


# 获取第二组数据 增加
def text2json(text):
    j2son = json.loads(json.loads(text)["data"])["chinaAdd"]
    return j2son


# 解析数据并生成返回数据
def result_json(text):
    j1son = text1json(r_text)
    j2son = text2json(r_text)
    # 现有数据
    confirm = j1son["confirm"]  # 确诊
    suspect = j1son["suspect"]  # 疑似
    dead = j1son["dead"]  # 死亡
    heal = j1son["heal"]  # 治愈
    # 较昨日增加
    confirm_add = j2son["confirm"]
    suspect_add = j2son["suspect"]
    dead_add = j2son["dead"]
    heal_add = j2son["heal"]
    result_j2 = {"code": "0", "confirm": str(confirm), "suspect": str(suspect), "dead": str(dead), "heal": str(heal)
                 , "confirm_add": str(confirm_add), "suspect_add": str(suspect_add), "dead_add": str(dead_add)
                 , "heal_add": str(heal_add)
                 }
    return json.dumps(result_j2)


@app.route('/json', methods=['GET'])
def r_json():
    if request.method == 'GET':
        ip = request.remote_addr
        result_str = result_json(r_text)
        print(f'\r\n[system]来自{ip}的GET请求：s_member_phone;return:{result_str}')
        return result_str


# 主入口
if __name__ == '__main__':
    try:
        app.run(host='0.0.0.0', port='2334', debug=True)
    except:
        print("发生错误！")
