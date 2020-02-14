# pneumonia
## 新型冠状肺炎情况实时显示  
## 使用库requests，json，flask  
## sketch_jan28a.ino 是显示屏的源程序  如想使用视频中演示效果 请替换 u8g2库  ArduinoJson库请使用5.x.x版本  

#### 服务端接口定义：GET  /json
####  返回结果

参数名称						|类型		|出现要求	|描述  
:----						|:---		|:------	|:---	
Code						|string		|R			|响应码
confirm							|string		|R			|现存确诊数
confirm_add						|string		|R			|较昨日确诊数
suspect				|string		|R			|现存疑似病例
suspect_add				|string		|R			|较昨日疑似病例
dead				|string		|R			|死亡数
dead_add					|string		|R			|较昨日死亡数
heal				|string		|R			|治愈数
heal_add					|string		|R			|较昨日治愈数



示例：

```
{
"code": "0",
"confirm": "63932",
"suspect": "10109",
"dead": "1381",
"heal": "7019",
"confirm_add": "4050",
"suspect_add": "-3326", 
"dead_add": "13", 
"heal_add": "1104"
}
```  
### 2020-02-14：  
1.更新了接口定义，服务端可用作其他地方  
### 2020-02-13：  
1.更换了数据  
2.C#服务端更换为python服务端  
3.已打包服务端可一键启动 dist目录下  
4.硬件部分更换为4屏幕显示模拟网页显示的效果  
