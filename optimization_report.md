# SNANDer CH347 优化报告 (macOS)

## 环境
- **编程器**: CH347F (USB-SPI)
- **系统**: macOS (libusb/darwin)
- **SPI NOR**: W25Q128BV (16MB)
- **SPI NAND**: W25N01GV (128MB)

## 优化历程

### SPI NOR 写入优化
| 版本 | 写入时间 | 速度 | 改进 |
|------|---------|------|------|
| 初始 | 251s | 66 KB/s | - |
| 优化1: 包打包+减少unprotect | 215s | 78 KB/s | 14% |
| 优化2: 自适应轮询 | 78s | 215 KB/s | **69%** |
| 最新 (2026-01-18) | 73s | 224 KB/s | **71%** |

### 主要优化点
1. **写入包打包**: opcode + 地址 + 256B 数据合并到单次 USB 传输
2. **减少冗余操作**: `snor_unprotect()` 移出写入循环
3. **自适应 WIP 轮询**: 渐进延迟减少 USB 往返
4. **SR_EPE 错误检测**: 写入/擦除错误时打印警告
5. **NAND OIP 轮询优化**: 添加自适应延迟和 5 秒超时保护
6. **SPI 速度可选**: `-S` 参数支持 60M/30M/15M/10M/5M/2M

## 最新测试结果 (2026-01-18)

### SPI NOR: W25Q128BV (16MB)
| 操作 | 时间 | 速度 |
|------|------|------|
| 读取 | 2s | 8 MB/s |
| 擦除 | 33s | - |
| 写入 | 73s | 224 KB/s |
| 验证 | 2s | 8 MB/s |

### SPI NAND: W25N01GV (128MB)
| 操作 | 时间 | 速度 |
|------|------|------|
| 读取 | 115s | 1.1 MB/s |
| 擦除 | 3s | 43 MB/s |
| 写入 | 318s | 412 KB/s |
| 验证 | 115s | 1.1 MB/s |

## 用法

```bash
# 识别芯片
./build/snander -i

# 全片擦除
./build/snander -e

# 写入并验证
./build/snander -w firmware.bin -v

# 指定 SPI 速度
./build/snander -w firmware.bin -v -S 30M

# 启用写入计时 (NOR)
./build/snander -w firmware.bin -v -t

# NAND 禁用 ECC 模式
./build/snander -w firmware.bin -v -d
```

## 性能瓶颈分析

CH347 写入比 T48 慢的原因：
1. **主机侧轮询**: 每页写入后需状态轮询 (WIP/OIP)，产生大量 USB 往返
2. **USB 传输开销**: 每包需确认回包，macOS 调度敏感
3. T48 将写入和轮询下放到设备固件，主机只发送数据块

## 改进建议
- macOS 上使用 `sudo` 可避免 LIBUSB_ERROR_ACCESS
- 写入时间主要受页编程等待限制，SPI 时钟主要影响读取速度

---

## 测试示例

### 1. 芯片识别 (Chip Probe)

#### SPI NOR (W25Q128BV)
```bash
$ ./build/snander -i
...
Using interface 4 for CH347F
Device revision is 1.2.0
SPI speed set to 60M.
spi device id: ef 40 18 0 0 (40180000)
Detected SPI NOR Flash: W25Q128BV, Flash Size: 16 MB
```

#### SPI NAND (W25N01GV)
```bash
$ ./build/snander -i
...
Using interface 4 for CH347F
Device revision is 1.2.0
SPI speed set to 60M.
spi device id: ff ef aa 21 0 (efaa2100)
SPI NOR Flash Not Detected!
spi_nand_probe: mfr_id = 0xef, dev_id = 0xaa, dev_id_2 = 0x21
Detected SPI NAND Flash: WINBOND W25N01GV, Flash Size: 128MB, OOB Size: 64B
```

### 2. SPI NOR (16MB) 写入并验证
```bash
$ ./build/snander -w test_16M.bin -v
...
Detected SPI NOR Flash: W25Q128BV, Flash Size: 16 MB
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000001000000
Written 100% [16777216] of [16777216] bytes      
Elapsed time: 73 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000001000000
Read 100% [16777216] of [16777216] bytes      
Elapsed time: 2 seconds
Status: OK
```

### 2. SPI NAND (W25N01GV 128MB) 写入并验证
```bash
$ ./build/snander -w test_128M.bin -v
...
Detected SPI NAND Flash: WINBOND W25N01GV, Flash Size: 128MB, OOB Size: 64B
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000008000000
Written 100% [134217728] of [134217728] bytes      
Elapsed time: 318 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000008000000
Read 100% [134217728] of [134217728] bytes      
Elapsed time: 115 seconds
Status: OK
```
