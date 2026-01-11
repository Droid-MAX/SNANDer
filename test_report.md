SNANDer-ch347 测试报告（macOS, CH347F, W25Q128BV）

日期：2026-01-10

范围
- 项目：SNANDer-ch347
- 目标设备：SPI NOR Flash（W25Q128BV，16 MB）
- 接口：CH347F（USB）
- 操作系统：macOS（libusb/darwin）

测试环境说明
- macOS libusb 提示未授权的 USB 捕获权限，非 sudo 场景可能无法完全分离内核驱动。
- 写入测试未使用 sudo；擦除/读取使用 sudo。

测试数据
- 文件：test_16M_random.bin
- 大小：16,777,216 字节（16 MB）
- MD5（源文件）：380756b34edb78a29c94c4fa3e4155e7

执行命令
1) 擦除
   sudo ./build/snander -e

2) 写入 + 校验
   ./build/snander -w test_16M_random.bin -v

3) 读取
   sudo ./build/snander -r test_16M_random-read.bin

结果
- 擦除：
  - 芯片：W25Q128BV，容量 16 MB
  - 用时：33 秒
  - 状态：OK

- 写入：
  - 写入长度：16 MB
  - 用时：217 秒
  - 吞吐：约 0.074 MB/s
  - 状态：OK

- 校验：
  - 校验读用时：3 秒
  - 状态：OK

- 读取（单独读取）：
  - 读取长度：16 MB
  - 用时：3 秒
  - 吞吐：约 5.3 MB/s
  - 状态：OK

- 完整性：
  - 读回文件 MD5 与源文件一致

追加测试（2MB）
- 预先全擦：
  - 用时：34 秒
  - 状态：OK
- 写入 + 校验：
  - 写入长度：2 MB
  - 用时：27 秒
  - 状态：OK
  - Timing:
    - usb/command 3.439 秒
    - wait 20.699 秒
    - other 3.151 秒
    - pages 8192, bytes 2097152

结论
- 写入耗时主要由芯片内部页编程等待决定（wait 占比约 76%）
- USB 传输耗时不是主要瓶颈（约 12%）
- 写入前未擦除会导致校验失败
 - 细分统计显示 wait 中 poll 与 sleep 比例接近，进一步优化空间有限

观察
- 读取速度正常且较快，写入速度明显偏慢。
- 非 sudo 写入时出现 libusb 访问/驱动分离警告：
  - "Cannot detach the existing USB driver. Claiming the interface may fail. LIBUSB_ERROR_ACCESS"
- 写入耗时符合 NOR Flash 页编程 + 频繁轮询的时间量级。

分析（可能原因）
- SPI NOR 写入吞吐主要受页编程时间（tPP）与状态轮询影响，而不是 SPI 60MHz 时钟本身。
- 每页写入后需要等待 WIP 清除，轮询需多次 USB 往返，macOS 上调度和 libusb 开销更明显。
- 写入未使用 sudo，可能导致驱动未完全分离，影响写入性能。

建议
1) 用 sudo 重新执行写入测试，比较吞吐是否改善。
2) 降低 macOS 下的状态轮询频率（已在代码中调整）。
3) 使用异步/流水线 USB 传输（已在代码中启用）。
4) 若仍需更高写速，考虑减少状态寄存器轮询次数或合并 USB 传输。

原始输出（用户提供）
- 擦除：用时 33 秒，状态 OK
- 写入：用时 217 秒，状态 OK
- 校验：读取用时 3 秒，状态 OK
- 读取：用时 3 秒，状态 OK
- MD5：源文件与读回一致
