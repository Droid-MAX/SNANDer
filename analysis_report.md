SNANDer-ch347 与 minipro(T48) 写入性能分析（macOS）

日期：2026-01-10

结论摘要
- CH347 写入慢的主要原因是主机侧进行页编程轮询，产生大量 USB 往返。
- T48 把写流程和轮询下放到设备固件，主机侧只发送数据块，USB 交互更少。
- CH347 在 Windows 上更快，是因为驱动/USB 传输路径更高效，但本质仍是主机轮询。

测试与观察（用户提供）
- 设备：W25Q128BV（16 MB）
- CH347F + macOS：
  - 擦除 33s
  - 写入 217s（约 0.074 MB/s）
  - 读取 3s（约 5.3 MB/s）
  - 校验 OK，MD5 一致
- T48 + macOS：
  - 写入约 50s（约 0.32 MB/s）

CH347 写入慢的原因（代码级分析）
1) 主机侧频繁轮询
   - SNOR 写入每页执行 snor_wait_ready()
   - 每次轮询包含写状态命令 + 读状态（两次 USB 传输）
   - 位置：src/spi_nor_flash.c

2) USB 传输开销高
   - ch347_write() 每包 OUT + IN 回包确认
   - 同步传输对 macOS 调度敏感
   - 位置：src/ch347_spi.c

3) macOS 权限/驱动分离影响
   - 非 sudo 写入时出现 LIBUSB_ERROR_ACCESS
   - 可能导致接口未完全分离、传输效率下降

为什么 T48 快
1) 写流程和轮询在设备固件内完成
   - 主机仅发送写命令和数据块
   - 无需主机侧轮询
   - 位置：minipro/src/t48.c（t48_write_block）

2) 协议一次传输更大块数据
   - write_buffer_size = 0x100（256B）与页一致
   - read_buffer_size = 0x1000（4KB）
   - 位置：minipro/infoic.xml（W25Q128BV）

3) SPI 时钟不是决定因素
   - T48 默认 SPI 8 MHz（低于 CH347 60 MHz）
   - 说明瓶颈是轮询与 USB 往返而非 SPI 时钟
   - 位置：minipro/src/database.c（DEFAULT_T48_SPI_CLOCK）

CH347 在 Windows 更快的原因
- 同样是主机轮询，但 WCH 官方 DLL/驱动减少了 USB 往返开销
- Windows USB 栈对频繁轮询更友好

是否存在“无需轮询”的方案
- 对 SPI NOR 必须等待芯片内部编程完成（WIP 位）
- CH347 作为 USB-SPI 桥没有固件编程引擎，无法避免主机等待
- 可行方案是“将轮询放到设备端”（如 T48 固件），或者用固定延时替代高频轮询

已实施的优化（macOS）
- 写入轮询间隔加大，减少 USB 往返：
  - NOR 轮询间隔增至 1 ms（macOS）
  - NAND 写页 OIP 轮询增加 1 ms 休眠
- 启用 CH347 异步/流水线 USB 传输

改进建议
1) 写入使用 sudo，避免 LIBUSB_ERROR_ACCESS
2) 进一步减少状态轮询次数（先 sleep 再轮询）
3) 合并状态寄存器读写，减少 USB 往返
4) 若可能，考虑使用带固件编程引擎的编程器（T48/T56/T76）
