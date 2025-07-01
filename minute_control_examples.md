# 分钟级别控制使用示例

## 🕐 时间设置方式

### 1. Web界面设置

#### 直接输入方式
- **分钟输入框**：设置0-60分钟
- **秒输入框**：设置0-59秒
- 两个输入框可以组合使用

#### 快捷按钮方式
- **30秒**：快速设置30秒
- **1分钟**：快速设置1分钟
- **2分钟**：快速设置2分钟
- **5分钟**：快速设置5分钟
- **10分钟**：快速设置10分钟

### 2. API调用方式

#### 传统方式（仅秒）
```bash
# 30秒正转
curl "http://192.168.1.100/advanced?action=timed&direction=cw&duration=30"

# 5分钟反转（300秒）
curl "http://192.168.1.100/advanced?action=timed&direction=ccw&duration=300"
```

#### 新方式（分钟+秒）
```bash
# 1分钟正转
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=1"

# 2分钟30秒反转
curl "http://192.168.1.100/advanced?action=timed&direction=ccw&minutes=2&seconds=30"

# 仅30秒正转
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=0&seconds=30"
```

## 📋 常用时间设置示例

### 短时间应用（测试用）
- **10秒**：快速功能测试
- **30秒**：基本功能验证
- **1分钟**：完整功能测试

### 中等时间应用（一般用途）
- **2分钟**：搅拌应用
- **5分钟**：混合应用
- **10分钟**：持续运行测试

### 长时间应用（特殊用途）
- **15分钟**：长时间搅拌
- **30分钟**：持续混合
- **60分钟**：最长连续运行

## 🎯 应用场景示例

### 1. 咖啡搅拌器
```javascript
// 设置2分钟搅拌
setQuickTime(2, 0);
startTimed(true); // 正转2分钟
```

### 2. 展示转台
```bash
# 每5分钟转一次，持续30秒
curl "http://192.168.1.100/advanced?action=timed&direction=cw&seconds=30"
# 等待4分30秒后重复
```

### 3. 实验室搅拌
```bash
# 高速搅拌1分钟
curl "http://192.168.1.100/advanced?speed=80"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=1"

# 低速搅拌5分钟
curl "http://192.168.1.100/advanced?speed=30"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=5"
```

### 4. 自动化生产线
```python
import requests
import time

def production_cycle(ip, cycles=10):
    """生产线自动化示例"""
    for i in range(cycles):
        print(f"生产周期 {i+1}/{cycles}")
        
        # 正转2分钟（加工阶段）
        requests.get(f"http://{ip}/advanced?action=timed&direction=cw&minutes=2")
        time.sleep(125)  # 等待完成 + 5秒缓冲
        
        # 反转30秒（复位阶段）
        requests.get(f"http://{ip}/advanced?action=timed&direction=ccw&seconds=30")
        time.sleep(35)   # 等待完成 + 5秒缓冲
        
        print(f"周期 {i+1} 完成")

# 使用示例
production_cycle("192.168.1.100", 5)
```

## ⚙️ 高级控制技巧

### 1. 渐进式时间控制
```bash
# 从短时间开始测试
curl "http://192.168.1.100/advanced?action=timed&direction=cw&seconds=10"
# 逐步增加时间
curl "http://192.168.1.100/advanced?action=timed&direction=cw&seconds=30"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=1"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=2"
```

### 2. 组合速度和时间控制
```bash
# 设置不同速度的时间段
# 高速30秒
curl "http://192.168.1.100/advanced?speed=90"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&seconds=30"

# 等待完成后，中速2分钟
sleep 35
curl "http://192.168.1.100/advanced?speed=50"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=2"

# 等待完成后，低速5分钟
sleep 125
curl "http://192.168.1.100/advanced?speed=20"
curl "http://192.168.1.100/advanced?action=timed&direction=cw&minutes=5"
```

### 3. 间歇式运行
```python
import requests
import time

def intermittent_operation(ip, work_minutes=1, rest_minutes=2, cycles=5):
    """间歇式运行：工作1分钟，休息2分钟，重复5次"""
    for i in range(cycles):
        print(f"工作周期 {i+1}/{cycles}")
        
        # 工作阶段
        requests.get(f"http://{ip}/advanced?action=timed&direction=cw&minutes={work_minutes}")
        time.sleep(work_minutes * 60 + 5)  # 等待工作完成
        
        print(f"休息 {rest_minutes} 分钟...")
        time.sleep(rest_minutes * 60)  # 休息时间
        
    print("间歇式运行完成")

# 使用示例
intermittent_operation("192.168.1.100", 1, 2, 5)
```

## 🔍 时间显示格式

### 串口输出格式
- **仅秒**：`开始定时旋转 30秒 正转`
- **仅分钟**：`开始定时旋转 5分钟 正转`
- **分钟+秒**：`开始定时旋转 2分30秒 正转`

### Web界面显示格式
- **最后命令**：`定时旋转 2分30秒 正转`
- **状态更新**：实时显示当前运行状态

## 📊 性能考虑

### 时间精度
- **短时间（<1分钟）**：精度 ±0.5秒
- **中等时间（1-10分钟）**：精度 ±1秒
- **长时间（>10分钟）**：精度 ±2秒

### 系统限制
- **最长时间**：60分钟（3600秒）
- **最短时间**：1秒
- **并发限制**：同时只能运行一个定时任务
- **内存使用**：长时间运行不会增加内存消耗

### 建议使用范围
- **日常使用**：1秒 - 10分钟
- **特殊应用**：10分钟 - 30分钟
- **极限使用**：30分钟 - 60分钟

## 🛠️ 故障排除

### 时间设置问题
1. **时间无效**：检查分钟(0-60)和秒(0-59)范围
2. **时间过长**：确认总时间不超过60分钟
3. **时间过短**：确认至少设置1秒

### 运行异常
1. **提前停止**：检查是否手动停止或网络中断
2. **时间不准**：检查系统时钟和网络延迟
3. **无响应**：检查WiFi连接和设备状态

### 性能优化
1. **长时间运行**：定期检查设备温度
2. **频繁切换**：避免过于频繁的启停操作
3. **网络稳定**：确保WiFi连接稳定
