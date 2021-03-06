import requests
import json
from flask import Flask, request

app = Flask(__name__)

url = 'https://view.inews.qq.com/g2/getOnsInfo?name=disease_h5'


# 获取第一组数据 现有
# def text1json(j1text):
#     j1son = json.loads(json.loads(j1text)["data"])["chinaTotal"]
#     return j1son


# 获取第二组数据 增加
# def text2json(j2text):
#     j2son = json.loads(json.loads(j2text)["data"])["chinaAdd"]
#     return j2son


# 解析数据并生成返回数据
def result_json():
    response = requests.get(url)
    r_text = response.text
    j1son = json.loads(json.loads(r_text)["data"])["chinaTotal"]
    j2son = json.loads(json.loads(r_text)["data"])["chinaAdd"]
    # 现有数据
    suspect = j1son["suspect"]  # 疑似
    dead = j1son["dead"]  # 死亡
    heal = j1son["heal"]  # 治愈
    confirm = (j1son["confirm"] - dead - heal)  # 确诊
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
        result_str = result_json()
        print(f'\r\n[system]来自{ip}的GET请求：r_json;return:{result_str}')
        return result_str


# 主入口
if __name__ == '__main__':
    try:
        app.run(host='0.0.0.0', port='2334', debug=True)
    except:
        print("发生错误！")
