using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Strcoll
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        Thread myThread;
        public delegate void MyDelegateUI(); //定义委托类型
        MyDelegateUI myDelegateUI; //声明委托对象
        private HttpListener m_http_listener = new HttpListener();
        private Thread m_http_thread = null;
        Task MyTask = null;
        static string url = "http://172.21.0.10:2334/";//IP替换为你局域网IP
        static string qz, ys = "";


        //private void button1_Click(object sender, EventArgs e)
        //{
        //    string a1 = "#剪刀";
        //    string[] a11 = new string[] {"石头","剪刀","布"};

        //    for (int i = 0; i < a11.Length; i++)
        //    {
        //        if (a1.Contains(a11[i]))
        //        {
        //            MessageBox.Show(i + "");
        //        }
        //    }

        //   }

        void doWork()
        {
            while (true)
            {
                Gets();
                Thread.Sleep(10000);

            }

                



            }
        








        void Gets()
        {
            string data = HttpGet("http://m.medsci.cn/wh.asp");
            string a11 = "确诊 ";
            string a12 = "疑似 ";
            qz = data.Substring(data.LastIndexOf(a11) + 3, 5);
            ys = data.Substring(data.LastIndexOf(a12) + 3, 5);

            lab_qz.Text = qz.Trim();
            lab_ys.Text = ys.Trim() ;
        }

        public static string ResponseA3(string qz,string ys)
        {

            Random random = new Random();
            var dic = new SortedDictionary<string,object>
                            {
                            {"code", "100"},
                            {"message", "ok"},
                            { "qz",Convert.ToInt32(qz) },
                            {"ys",Convert.ToInt32(ys)}
                            };
            var jsonParam = JsonConvert.SerializeObject(dic);
            return jsonParam;
        }


        public static string HttpGet(string url)
        {
            WebRequest myWebRequest = WebRequest.Create(url);
            WebResponse myWebResponse = myWebRequest.GetResponse();
            Stream ReceiveStream = myWebResponse.GetResponseStream();
            string responseStr = "";
            if (ReceiveStream != null)
            {
                StreamReader reader = new StreamReader(ReceiveStream, Encoding.UTF8);
                responseStr = reader.ReadToEnd();
                reader.Close();
            }
            myWebResponse.Close();
            return responseStr;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Label.CheckForIllegalCrossThreadCalls = false;
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            myThread = new Thread(doWork);
            myDelegateUI = new MyDelegateUI(Gets);//绑定委托

            myThread.Start();
            QueryStart(MyTask, url);
            //string qz, ys;
            //qz = ys = "";
            //if (Gets(HttpGet("http://m.medsci.cn/wh.asp"), out qz, out ys) == "0")
            //{
            //    lab_qz.Text = qz.Trim();
            //    lab_ys.Text = ys.Trim();
            //}


        }

        protected virtual void QueryHttpServiceThd()
        {
            while (true)
            {
                try
                {
                    HttpListenerContext context = m_http_listener.GetContext();
                    HttpListenerRequest request = context.Request;
                    HttpListenerResponse response = context.Response;
                    
                    switch (context.Request.QueryString["Mode"])
                    {
                        default:
                            response.StatusCode = 500; 
                            break;
                        case "1":
                            response.StatusCode = 200;
                            response.AddHeader("Access-Control-Allow-Origin", "*");
                            using (StreamWriter writer = new StreamWriter(context.Response.OutputStream, Encoding.UTF8))
                            {
                                writer.Write(ResponseA3(qz.Trim(),ys.Trim()));
                             }
                            break;
                        case "qz":
                            response.StatusCode = 200;
                            response.AddHeader("Access-Control-Allow-Origin", "*");
                            using (StreamWriter writer = new StreamWriter(context.Response.OutputStream, Encoding.UTF8))
                            {
                                writer.Write(qz.Trim());
                            }
                            break;
                        case "ys":
                            response.StatusCode = 200;
                            response.AddHeader("Access-Control-Allow-Origin", "*");
                            using (StreamWriter writer = new StreamWriter(context.Response.OutputStream, Encoding.UTF8))
                            {
                                writer.Write(ys.Trim());
                            }
                            break;

                    }

                }
                catch (Exception ex)
                {
                    
                }
            }
        }

        public bool QueryStart(Task MyTask, string url)
        {
            this.MyTask = MyTask;
            if (!HttpListener.IsSupported)
            {

                return false;
            }
            if (m_http_thread != null)
                return true;

            string http_url = url;
            try
            {
                m_http_listener.AuthenticationSchemes = AuthenticationSchemes.Anonymous;
                m_http_listener.Prefixes.Add(http_url);
                m_http_listener.Start();

                m_http_thread = new Thread(QueryHttpServiceThd);
                m_http_thread.Name = "HttpServiceThd";
                m_http_thread.IsBackground = true;
                m_http_thread.Start();

            }
            catch (Exception ex)
            {
                
            }

            return false;
        }

    }
}
