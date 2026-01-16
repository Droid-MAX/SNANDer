# SNANDer-ch347 Change/Test Log Index

This file now points to the consolidated report in `optimization_report.md`.

Quick links:
- Consolidated results and analysis: `optimization_report.md`
- Short test report index: `test_report.md`

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
