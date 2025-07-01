#include "secrets.h"
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>



// ULN2003 步进电机控制引脚定义 (更新为您的引脚配置)
#define IN1 1   // ULN2003 输入1 连接到 ESP32 GPIO1
#define IN2 12  // ULN2003 输入2 连接到 ESP32 GPIO12
#define IN3 18  // ULN2003 输入3 连接到 ESP32 GPIO18
#define IN4 19  // ULN2003 输入4 连接到 ESP32 GPIO19

// 步进电机参数
const int stepsPerRevolution = 2048;  // 28BYJ-48步进电机每转一圈的步数
const int stepDelay = 2;              // 步进间隔时间(毫秒)

// 步进序列 - 8步序列(半步模式，更平滑)
/**
 * @brief Step sequence for controlling a stepper motor in full-step mode.
 *
 * This 2D array defines the step pattern for a 4-wire stepper motor.
 * Each row represents a step position, with 4 columns corresponding to the motor coils.
 * A value of 1 activates a coil, 0 deactivates it.
 * The sequence provides smooth rotation in both directions.
 */
int stepSequence[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}};

// 全局变量
int currentStep = 0;
bool isRunning = false;  // 电机运行状态
String lastCommand = ""; // 最后执行的命令

// 创建Web服务器对象
AsyncWebServer server(80);

// 函数声明
void stepMotor(int steps, bool clockwise = true);
void setStep(int step);
void stopMotor();
void setupWiFi();
void setupWebServer();
String getWebPage();
void executeCommand(String command);

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 ULN2003 步进电机WiFi控制器启动");

  // 初始化控制引脚
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // 初始状态关闭所有线圈
  stopMotor();
  Serial.println("引脚初始化完成");

  // 初始化WiFi
  setupWiFi();

  // 初始化Web服务器
  setupWebServer();

  Serial.println("系统初始化完成");
  Serial.println("串口命令说明:");
  Serial.println("  'f' - 正转一圈");
  Serial.println("  'b' - 反转一圈");
  Serial.println("  's' - 停止电机");
  Serial.println("  'h' - 正转半圈");
  Serial.println("  'r' - 反转半圈");
}

void loop() {
  // 检查串口命令
  if (Serial.available()) {
    char command = Serial.read();
    executeCommand(String(command));
  }

  // 处理其他任务
  delay(10);
}

// 步进电机控制函数
void stepMotor(int steps, bool clockwise) {
  isRunning = true;
  for (int i = 0; i < steps; i++) {
    if (clockwise) {
      currentStep = (currentStep + 1) % 8;
    } else {
      currentStep = (currentStep - 1 + 8) % 8;
    }

    setStep(currentStep);
    delay(stepDelay);
  }

  // 运行完成后停止电机以节省功耗
  stopMotor();
  isRunning = false;
}

// 设置步进状态
void setStep(int step) {
  digitalWrite(IN1, stepSequence[step][0]);
  digitalWrite(IN2, stepSequence[step][1]);
  digitalWrite(IN3, stepSequence[step][2]);
  digitalWrite(IN4, stepSequence[step][3]);
}

// 停止电机（关闭所有线圈）
void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  isRunning = false;
}

// 执行命令函数
void executeCommand(String command) {
  command.trim();
  command.toLowerCase();

  if (command == "f") {
    Serial.println("正转一圈...");
    lastCommand = "正转一圈";
    stepMotor(stepsPerRevolution, true);
    Serial.println("正转完成");
  }
  else if (command == "b") {
    Serial.println("反转一圈...");
    lastCommand = "反转一圈";
    stepMotor(stepsPerRevolution, false);
    Serial.println("反转完成");
  }
  else if (command == "h") {
    Serial.println("正转半圈...");
    lastCommand = "正转半圈";
    stepMotor(stepsPerRevolution / 2, true);
    Serial.println("正转半圈完成");
  }
  else if (command == "r") {
    Serial.println("反转半圈...");
    lastCommand = "反转半圈";
    stepMotor(stepsPerRevolution / 2, false);
    Serial.println("反转半圈完成");
  }
  else if (command == "q") {
    Serial.println("正转90度...");
    lastCommand = "正转90度";
    stepMotor(stepsPerRevolution / 4, true);
    Serial.println("正转90度完成");
  }
  else if (command == "e") {
    Serial.println("反转90度...");
    lastCommand = "反转90度";
    stepMotor(stepsPerRevolution / 4, false);
    Serial.println("反转90度完成");
  }
  else if (command == "s") {
    Serial.println("停止电机");
    lastCommand = "停止电机";
    stopMotor();
  }
  else {
    Serial.println("未知命令: " + command);
  }
}

// WiFi初始化函数
void setupWiFi() {
  Serial.println("正在连接WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi连接成功!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    Serial.print("请在浏览器中访问: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("WiFi连接失败!");
    Serial.println("请检查WiFi名称和密码设置");
  }
}

// Web服务器初始化函数
void setupWebServer() {


  // 控制命令路由
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    String command = "";
    if (request->hasParam("cmd")) {
      command = request->getParam("cmd")->value();
      executeCommand(command);
    }
    request->send(200, "text/plain", "OK");
  });

  // 状态查询路由
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    String status = "{";
    status += "\"running\":" + String(isRunning ? "true" : "false") + ",";
    status += "\"lastCommand\":\"" + lastCommand + "\"";
    status += "}";
    request->send(200, "application/json", status);
  });
  // 主页路由
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", getWebPage()); });
  server.begin();
  Serial.println("Web服务器已启动");
}

// 生成Web页面HTML
String getWebPage() {
  String html = R"(
<!DOCTYPE html>
<html lang='zh-CN'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>ESP32 步进电机控制器</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Microsoft YaHei', Arial, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .container {
            background: rgba(255, 255, 255, 0.95);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0,0,0,0.1);
            max-width: 600px;
            width: 100%;
            text-align: center;
        }

        h1 {
            color: #333;
            margin-bottom: 10px;
            font-size: 2.5em;
            font-weight: 300;
        }

        .subtitle {
            color: #666;
            margin-bottom: 40px;
            font-size: 1.1em;
        }

        .status {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin-bottom: 30px;
            border-left: 4px solid #007bff;
        }

        .status h3 {
            color: #333;
            margin-bottom: 10px;
        }

        .status-info {
            display: flex;
            justify-content: space-between;
            align-items: center;
            flex-wrap: wrap;
            gap: 10px;
        }

        .status-item {
            background: white;
            padding: 10px 15px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            flex: 1;
            min-width: 120px;
        }

        .status-label {
            font-size: 0.9em;
            color: #666;
            margin-bottom: 5px;
        }

        .status-value {
            font-weight: bold;
            color: #333;
        }

        .running {
            color: #28a745;
        }

        .stopped {
            color: #6c757d;
        }

        .controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
            margin-bottom: 30px;
        }

        .control-group {
            background: #f8f9fa;
            border-radius: 15px;
            padding: 25px;
            border: 2px solid #e9ecef;
            transition: all 0.3s ease;
        }

        .control-group:hover {
            border-color: #007bff;
            transform: translateY(-2px);
            box-shadow: 0 8px 25px rgba(0,123,255,0.15);
        }

        .control-group h3 {
            color: #333;
            margin-bottom: 20px;
            font-size: 1.3em;
        }

        .btn {
            background: linear-gradient(45deg, #007bff, #0056b3);
            color: white;
            border: none;
            padding: 15px 25px;
            border-radius: 10px;
            cursor: pointer;
            font-size: 1em;
            font-weight: 500;
            margin: 5px;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(0,123,255,0.3);
            width: 100%;
            max-width: 150px;
        }

        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(0,123,255,0.4);
        }

        .btn:active {
            transform: translateY(0);
        }

        .btn-danger {
            background: linear-gradient(45deg, #dc3545, #c82333);
            box-shadow: 0 4px 15px rgba(220,53,69,0.3);
        }

        .btn-danger:hover {
            box-shadow: 0 6px 20px rgba(220,53,69,0.4);
        }

        .btn-success {
            background: linear-gradient(45deg, #28a745, #1e7e34);
            box-shadow: 0 4px 15px rgba(40,167,69,0.3);
        }

        .btn-success:hover {
            box-shadow: 0 6px 20px rgba(40,167,69,0.4);
        }

        .btn-warning {
            background: linear-gradient(45deg, #ffc107, #e0a800);
            box-shadow: 0 4px 15px rgba(255,193,7,0.3);
            color: #212529;
        }

        .btn-warning:hover {
            box-shadow: 0 6px 20px rgba(255,193,7,0.4);
        }

        .footer {
            margin-top: 30px;
            padding-top: 20px;
            border-top: 1px solid #e9ecef;
            color: #666;
            font-size: 0.9em;
        }

        @media (max-width: 768px) {
            .container {
                padding: 20px;
                margin: 10px;
            }

            h1 {
                font-size: 2em;
            }

            .controls {
                grid-template-columns: 1fr;
            }

            .status-info {
                flex-direction: column;
            }

            .status-item {
                width: 100%;
            }
        }
    </style>
</head>
<body>
    <div class='container'>
        <h1>🔧 步进电机控制器</h1>
        <p class='subtitle'>ESP32 + ULN2003 + 28BYJ-48</p>

        <div class='status'>
            <h3>📊 系统状态</h3>
            <div class='status-info'>
                <div class='status-item'>
                    <div class='status-label'>运行状态</div>
                    <div class='status-value' id='motorStatus'>检查中...</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>最后命令</div>
                    <div class='status-value' id='lastCommand'>无</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>IP地址</div>
                    <div class='status-value'>)" + WiFi.localIP().toString() + R"(</div>
                </div>
            </div>
        </div>

        <div class='controls'>
            <div class='control-group'>
                <h3>🔄 完整旋转</h3>
                <button class='btn btn-success' onclick='sendCommand("f")'>正转一圈</button>
                <button class='btn btn-warning' onclick='sendCommand("b")'>反转一圈</button>
            </div>

            <div class='control-group'>
                <h3>↩️ 半圈旋转</h3>
                <button class='btn btn-success' onclick='sendCommand("h")'>正转半圈</button>
                <button class='btn btn-warning' onclick='sendCommand("r")'>反转半圈</button>
            </div>

            <div class='control-group'>
                <h3>🎯 精确控制</h3>
                <button class='btn btn-success' onclick='sendCommand("q")'>正转90°</button>
                <button class='btn btn-warning' onclick='sendCommand("e")'>反转90°</button>
            </div>

            <div class='control-group'>
                <h3>⏹️ 停止控制</h3>
                <button class='btn btn-danger' onclick='sendCommand("s")'>立即停止</button>
            </div>
        </div>

        <div class='footer'>
            <p>💡 提示：点击按钮控制步进电机，状态会实时更新</p>
            <p>🔧 硬件：ESP32-C3 + ULN2003 + 28BYJ-48</p>
        </div>
    </div>

    <script>
        function sendCommand(cmd) {
            fetch('/control?cmd=' + cmd)
                .then(response => response.text())
                .then(data => {
                    console.log('命令发送成功:', cmd);
                    updateStatus();
                })
                .catch(error => {
                    console.error('发送命令失败:', error);
                    alert('发送命令失败，请检查网络连接');
                });
        }

        function updateStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    const statusElement = document.getElementById('motorStatus');
                    const commandElement = document.getElementById('lastCommand');

                    if (data.running) {
                        statusElement.textContent = '运行中';
                        statusElement.className = 'status-value running';
                    } else {
                        statusElement.textContent = '已停止';
                        statusElement.className = 'status-value stopped';
                    }

                    commandElement.textContent = data.lastCommand || '无';
                })
                .catch(error => {
                    console.error('获取状态失败:', error);
                    document.getElementById('motorStatus').textContent = '连接错误';
                });
        }

        // 页面加载时更新状态
        updateStatus();

        // 每2秒更新一次状态
        setInterval(updateStatus, 2000);
    </script>
</body>
</html>
)";

  return html;
}