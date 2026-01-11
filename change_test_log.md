SNANDer-ch347 修改与测试记录（中文）

日期：2026-01-10

背景
- 设备：W25Q128BV（16 MB）
- 连接：CH347F
- 系统：macOS（libusb/darwin）
- 目标：分析与提升写入速度，并对比 T48(minipro) 的性能差异

已实施的修改（按时间顺序）
1) macOS 写入轮询降频
   - 将 NOR 状态轮询间隔设置为 1ms（macOS），减少 USB 往返
   - 文件：src/spi_nor_flash.c

2) NAND 写页轮询加间隔（macOS）
   - OIP 轮询间隔增加 1ms（macOS）
   - 文件：src/spi_nand_flash.c

3) 启用 CH347 异步/流水线 USB 传输
   - macOS/Linux 路径启用 libusb 异步传输
   - 写路径优先使用异步传输，失败回退同步
   - 文件：src/ch347_spi.c

4) 回滚固定延时写入策略
   - 曾在每页写后加入 usleep(2000)
   - 实测导致写入更慢后已移除
   - 文件：src/spi_nor_flash.c

5) 写入耗时统计日志
   - 增加写入统计日志开关（SNANDER_TIMING）
   - 统计 USB 传输耗时、等待写入耗时、总耗时
   - 文件：src/spi_nor_flash.c

已生成文档
- 测试报告：test_report.md
- 性能分析：analysis_report.md

测试与实验记录
1) 擦除（CH347F，sudo）
   - 命令：sudo ./build/snander -e
   - 结果：33s

2) 写入 + 校验（CH347F，非 sudo）
   - 命令：./build/snander -w test_16M_random.bin -v
   - 结果：217s
   - 读校验：3s

3) 读取（CH347F，sudo）
   - 命令：sudo ./build/snander -r test_16M_random-read.bin
   - 结果：3s
   - MD5 一致

4) 写入（CH347F，sudo）
   - 命令：sudo ./build/snander -w test_16M_random.bin
   - 结果：194s

5) 固定延时策略测试（每页写后 usleep(2000)）
   - 写入结果：284s（更慢）
   - 该策略已移除

6) 写入统计日志尝试
   - 命令：SNANDER_TIMING=1 sudo ./build/snander -w test_16M_random.bin -v
   - 现象：未出现 Timing 输出
   - 推测原因：未重新编译或 sudo 未继承环境变量
   - 建议：先 ./build-for-darwin.sh 再用 sudo -E 运行

7) 对比：T48(minipro) 写入
   - 结果：约 50s
   - 原因分析见 analysis_report.md

8) 2MB 写入计时（含 timing）
   - 命令：sudo ./build/snander -t -w test_2M_random.bin -v
   - 结果：27s
   - Timing:
     - usb/command 3.439s
     - wait 20.699s
     - other 3.151s
     - pages 8192, bytes 2097152

9) 2MB 写入前全擦
   - 命令：sudo ./build/snander -e
   - 结果：34s

10) 2MB 写入计时（非 sudo）
    - 命令：./build/snander -t -w test_2M_random.bin -v
    - 结果：26s
    - Timing:
      - usb/command 3.268s
      - wait 20.134s (poll 9.938s, sleep 10.196s, other 0.001s)
      - other 2.980s
      - pages 8192, bytes 2097152

结论（新增）
- wait 约占总耗时 77%，其中轮询 USB 与 sleep 各占约一半
- usb/command 仅占约 13%，说明写入瓶颈主要是页编程等待
- 进一步优化空间有限，除非将轮询移到设备端或更换硬件

当前结论
- CH347 写入慢的主要瓶颈是主机侧轮询带来的 USB 往返开销。
- T48 快是因为写流程与轮询在设备固件内完成，主机交互更少。

后续建议
1) 重新编译并使用 sudo -E 运行，确认 Timing 日志输出
2) 若 Timing 显示等待占比高，考虑更优化的轮询策略
3) 若 USB 占比高，继续优化传输合并或并行
