# 微處理機系統與介面技術 - LAB 4

## 實驗簡介
本次實驗利用 NUC140 微控制器與 SPI 通訊協定，實現與 ADXL345 三軸加速度感測器的通信，讀取三軸加速度數據，並透過校正計算將結果顯示於 Putty 終端。

## 實驗器材
- **NUC140 V2.0 開發板**
- **ADXL345 三軸加速度感測器**
- **UART to USB 模組**（PL2303HXA 或 CH340G）
- **Putty 軟體**（用於終端模擬）

## 實驗功能
### 基本功能
- 使用 SPI 與 ADXL345 通信，讀取 X、Y、Z 三軸加速度數據。
- 校正三軸數據並顯示於 Putty，每 500ms 輸出一次。

## 實驗步驟
1. **硬體接線**：
   - **CS** → GPD0 (SPI2 SS0)
   - **SCL** → GPD1 (SPI2 CLK)
   - **SDO** → GPD2 (SPI2 MISO)
   - **SDA** → GPD3 (SPI2 MOSI)
2. **軟體設置**：
   - 初始化 SPI2，配置主從模式及時鐘極性與相位（Mode 3）。
   - 設定 UART 波特率為 115200，作為終端輸出。
3. **程式運行**：
   - 初始化 ADXL345，讀取並校正三軸數據，結果顯示於 Putty。

## 系統架構
### 程式結構
- **main.c**：
  - `SYS_Init`：初始化系統時鐘及多功能腳位。
  - `SPI_Init`：設定 SPI2 為主機模式，配置時鐘頻率、極性與相位。
  - `ADXL_write`：向 ADXL 寫入暫存器值。
  - `ADXL_read`：從 ADXL 讀取暫存器值。
  - `ADXL_init`：配置 ADXL 初始參數。
  - `main`：主程式控制流程，包含數據讀取、校正及輸出。

### SPI 配置
- **SPI 工作模式**：Mode 3（CPOL=1, CPHA=1）
- **傳輸數據長度**：8 bits
- **時鐘頻率**：2 MHz

### ADXL 配置
- **POWER_CTL (0x2D)**：啟用測量模式。
- **DATA_FORMAT (0x31)**：設置 16 位資料格式。
- **FIFO_CTL (0x38)**：設置 FIFO 為 Stream 模式。

## 成果展示
- Putty 輸出範例：
- `x: -0.03, y: 0.01, z: 1.02`

