# 微處理機系統與介面技術 - LAB 2

## 實驗簡介
本次實驗利用 NUC140 微控制器實現 UART 通訊功能，將鍵盤輸入的字串傳送到 PC 的 Putty 終端，並在按下 Enter 鍵後顯示輸入內容。同時，根據輸入的特定命令字串控制 RGB LED 燈的開關。

## 實驗器材
- **NUC140 V2.0 開發板**
- **UART to USB 模組**（PL2303HXA 或 CH340G）
- **Putty 軟體**（用於終端模擬）

## 實驗功能
### 基本功能
- 接收鍵盤輸入的字元，按下 Enter 後顯示完整字串於終端。
- 輸入過程中終端不顯示字元，僅在 Enter 後顯示。

### 加分功能
- 根據特定字串命令控制 RGB LED：
  - `red on` → 開啟紅燈
  - `red off` → 關閉紅燈
  - `green on` → 開啟綠燈
  - `green off` → 關閉綠燈
  - `blue on` → 開啟藍燈
  - `blue off` → 關閉藍燈

## 實驗步驟
1. **硬體接線**：
   - UART0 的 RX（PB0）接 USB 模組的 TX，TX（PB1）接 USB 模組的 RX，GND 接 USB 模組的 GND。
2. **軟體設置**：
   - 使用 Putty 設定對應的 COM Port 和 Baud rate（9600）。
3. **程式運行**：
   - 上電運行程式，測試 UART 傳輸與 RGB LED 控制。

## 系統架構
### 程式結構
- **main.c**：
  - `SYS_Init`：初始化系統時鐘和多功能腳位。
  - `UART0_Init`：初始化 UART，包括波特率設置與中斷啟用。
  - `GPIO_Init`：設置 RGB LED 的 GPIO 腳位為輸出模式。
  - `ParseCommand`：解析輸入命令並控制 RGB LED。
  - `UART02_IRQHandler`：處理 UART RDA 與 THRE 中斷。
  
### 中斷處理
- **RDA 中斷**：當接收到新字元時將其存入緩衝區，遇到 Enter 結束並解析命令。
- **THRE 中斷**：逐字將緩衝區內容透過 UART 傳送至終端。

## 成果展示
- 在 Putty 中輸入字串後按下 Enter，字串正確顯示。
- 輸入 RGB 命令字串，LED 按命令正確開關。

