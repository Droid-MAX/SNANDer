这份文档基于您提供的终端日志整理而成。它详细说明了如何在 macOS（适用于 Linux/Windows）环境下，配合 `ch347_spi` 驱动使用 Flashrom 对 SPI Flash 芯片进行读写操作。

---

# CH347 Flashrom 使用教程 (SPI 模式)

本教程旨在指导用户使用基于 CH347 芯片的编程器配合 Flashrom 工具对 SPI Flash（如 BIOS 芯片、固件存储）进行烧录、读取和擦除。

## 1. 环境与硬件准备

* 编程器: CH347 (SPI 模式)
* 软件: Flashrom v1.6.0 (或支持 `ch347_spi` 协议的分支)
* 操作系统: macOS (本教程演示环境), Linux, Windows
* 目标芯片: SPI Flash (本例中使用 Winbond W25Q128.V, 容量 16MB)

> 注意: 在 macOS 和 Linux 上，操作 USB 设备通常需要管理员权限 (`sudo`)。

---

## 2. 常用命令详解

所有命令的基础格式如下:  
`sudo flashrom -p ch347_spi:spispeed=30M [操作指令]`

* `-p ch347_spi`: 指定编程器驱动为 CH347 SPI。
* `spispeed=30M`: 设置 SPI 时钟频率为 30MHz (可根据信号质量调整)。

### 2.1 擦除芯片 (Erase)

此操作将清空芯片内的所有数据（置为 `0xFF`）。

```bash
sudo flashrom -p ch347_spi:spispeed=30M -E
```

* 日志反馈: `Erase/write done from 0 to ffffff` 表示 16MB 空间擦除成功。

### 2.2 写入固件 (Write)

将本地文件写入芯片。Flashrom 会自动执行“读取旧内容 -> 擦除差异部分 -> 写入新内容 -> 校验”的流程。

```bash
sudo flashrom -p ch347_spi:spispeed=30M -w test_16M_random.bin
```

* 耗时参考: 16MB 文件写入约需 1分50秒 (包含读取、擦除、写入、校验)。
* 过程:
  1) `Reading old flash chip contents...` (读取旧数据)
  2) `Updating flash chip contents...` (写入新数据)
  3) `Verifying flash... VERIFIED.` (校验通过)

### 2.3 读取固件 (Read/Backup)

将芯片内的内容读取并保存到本地文件。

```bash
sudo flashrom -p ch347_spi:spispeed=30M -r test_16M_random-read.bin
```

* 耗时参考: 16MB 文件读取约需 5.2 秒 (速度极快，约 3MB/s)。

### 2.4 数据完整性校验

在读取完成后，建议校验读取文件的 MD5/SHA 值与源文件是否一致。

```bash
md5 test_16M_random-read.bin
md5 test_16M_random.bin
```

* 如果两个哈希值一致（如日志中的 `380756b3...`），则说明烧录和读取均完美无误。

---

## 3. 常见错误与避坑指南

在使用 Flashrom 时，参数的组合非常严格。以下是日志中出现的错误及其原因：

### 错误 1: `More than one operation specified`

* 错误命令: `sudo flashrom ... -w -v filename.bin`
* 原因: 同时指定了 `-w` (Write) 和 `-v` (Verify)。
* 解决: 仅使用 `-w`。Flashrom 在写入模式下默认会在写入完成后自动进行校验，无需额外添加 `-v` 参数。

### 错误 2: `Error: Extra parameter found`

* 错误命令: `sudo flashrom ... -w -V filename.bin`
* 原因: 参数格式错误或 `-V` (Verbose，大写) 位置/用法不当导致解析混乱。
* 解决: 遵循标准格式 `-w 文件名`。

### 提示: `libusb: info [darwin_detach_kernel_driver]...`

* 现象: macOS 下运行命令时出现 `no capture entitlements` 警告。
* 解释: 这是 macOS USB 权限管理的提示。只要后续出现 `Found Winbond flash chip...`，说明驱动已成功接管，可以忽略此警告。

---

## 4. 性能参考 (Benchmark)

基于 CH347 在 30MHz SPI 时钟下的实测数据（针对 16MB W25Q128 芯片）：

| 操作类型 | 数据量 | 耗时 | 平均速度 | 备注 |
| --- | --- | --- | --- | --- |
| 读取 (Read) | 16 MB | ~5.22 秒 | ~3.06 MB/s | 速度快，适合备份 |
| 写入 (Write) | 16 MB | ~109.9 秒 | ~0.145 MB/s | 包含读/擦/写/验全流程 |

总结: CH347 配合 Flashrom 使用时，读取速度较快，适合备份固件；写入速度中规中矩，但稳定性良好（自动校验通过）。

---

下一步建议:
- 如需自动化流程，可将“备份 -> 写入 -> 校验”整理成 Shell 脚本批量执行。
