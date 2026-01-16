# SNANDer Optimization & Performance Report (macOS)

## 1. Objective
Optimize the `snander` tool on macOS (specifically for CH347F USB interface) to resolve slow write speeds when programming SPI NOR Flash chips (W25Q128FV).

## 2. Optimization Strategy
The primary bottleneck was identified as excessive USB transaction overhead.
*   **Packet Bundling**: Refactored `snor_write` in `spi_nor_flash.c` to bundle the SPI Command (1 byte), Address (3/4 bytes), and Data Page (256 bytes) into a single USB Bulk Transfer.
*   **Reduced Overhead**: Removed redundant `unprotect` calls within the write loop.
*   **Pointer Fix**: Corrected data type mismatches in `ch347_spi.c` (`unsigned long` vs `int`) for `libusb` calls on macOS to prevent hangs.

## 3. Environment and Test Data
- Device: W25Q128BV (16MB)
- Programmer: CH347F (USB)
- OS: macOS (libusb/darwin)
- Test file: `test_16M_random.bin` (16,777,216 bytes)
- Note: non-sudo runs may show libusb access warnings and can affect stability.

## 4. Benchmark Results (16MB Full Chip Write)

The following tests were conducted on a **W25Q128FV (16MB)** chip.

| Metric | Initial (251s) | Optimized (latest) | Improvement |
| :--- | :--- | :--- | :--- |
| **Write Time** | 251 seconds | **78 seconds** | **-69%** |
| **Write Speed** | ~66.8 KB/s | **~205 KB/s** | **~3.1x** |
| **Verify Time** | N/A | **3 seconds** | **5.3 MB/s (Limiting factor: USB Read)** |
| **Status** | OK | **OK** | Stable with `sudo` |

**Note**: An earlier optimized pass reached **215s** (from 251s). The latest results reflect the current code and settings. Performance can vary based on system load during long operations.

### Latest Results (macOS, CH347F, W25Q128BV, 16MB)

| Metric | Default (60M) | 30M | Notes |
| :--- | :--- | :--- | :--- |
| **Write Time** | **78 seconds** | **85 seconds** | Write time changes little with SPI speed |
| **Verify Time** | **3 seconds** | **5 seconds** | Read slower at 30M |
| **Status** | OK | OK | Verified OK |

Timing breakdown with optimized polling (60M):

```
Timing: usb/command 17.340s, wait 51.571s (poll 49.722s, sleep 1.844s, other 0.005s), other 15.069s
```

Conclusion: write time is dominated by page-program/WIP polling; SPI clock affects read speed more than total write time.

### Consolidated Test Log (selected)

- Erase (sudo): 33s
- Write + verify (non-sudo): 217s write, 3s verify, OK
- Write (sudo): 194s
- 2MB write + verify (sudo, timing enabled): 27s
  - Timing: usb/command 3.439s, wait 20.699s, other 3.151s
- 2MB write + verify (non-sudo, timing enabled): 26s
  - Timing: usb/command 3.268s, wait 20.134s (poll 9.938s, sleep 10.196s), other 2.980s
- Write + verify (60M, latest): 78s write, 3s verify, OK
- Write + verify (30M): 85s write, 5s verify, OK
- Write + verify (60M, timing enabled): 84s write
  - Timing: usb/command 17.340s, wait 51.571s (poll 49.722s, sleep 1.844s), other 15.069s

### Detailed Logs

#### Before Optimization
```text
Cmd: ./releases/darwin-arm64/snander -w test_data.bin
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000001000000
Written 100% [16777216] of [16777216] bytes
Elapsed time: 251 seconds
Status: OK
```

#### After Optimization (with Verification)
```text
Cmd: sudo ./releases/darwin-arm64/snander -w test_16M.bin -v
WRITE:
Write addr = 0x0000000000000000, len = 0x0000000001000000
Written 100% [16777216] of [16777216] bytes
Elapsed time: 215 seconds
Status: OK
VERIFY:
Read addr = 0x0000000000000000, len = 0x0000000001000000
Read 100% [16777216] of [16777216] bytes
Elapsed time: 3 seconds
Status: OK
```

## 5. Stability Recommendations
*   **Use `sudo`**: On macOS, running `snander` with `sudo` is critical to prevent `LIBUSB_ERROR_ACCESS` and interface locking issues during intensive operations.
*   **Validation**: The new verify speed (3 seconds for 16MB) confirms that reading is operating at very high efficiency (~5.3 MB/s).

## 6. Related Documents
- `test_report.md` (index)
- `change_test_log.md` (index)
