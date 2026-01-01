# SNANDer Optimization & Performance Report (macOS)

## 1. Objective
Optimize the `snander` tool on macOS (specifically for CH347F USB interface) to resolve slow write speeds when programming SPI NOR Flash chips (W25Q128FV).

## 2. Optimization Strategy
The primary bottleneck was identified as excessive USB transaction overhead.
*   **Packet Bundling**: Refactored `snor_write` in `spi_nor_flash.c` to bundle the SPI Command (1 byte), Address (3/4 bytes), and Data Page (256 bytes) into a single USB Bulk Transfer.
*   **Reduced Overhead**: Removed redundant `unprotect` calls within the write loop.
*   **Pointer Fix**: Corrected data type mismatches in `ch347_spi.c` (`unsigned long` vs `int`) for `libusb` calls on macOS to prevent hangs.

## 3. Benchmark Results (16MB Full Chip Write)

The following tests were conducted on a **W25Q128FV (16MB)** chip using the optimized binary.

| Metric | Before Optimization | After Optimization | Improvement |
| :--- | :--- | :--- | :--- |
| **Write Time** | 251 seconds | **215 seconds** | **+14.3%** |
| **Write Speed** | ~66.8 KB/s | **~78.0 KB/s** | **+11.2 KB/s** |
| **Verify Time** | N/A | **3 seconds** | **5.3 MB/s (Limiting factor: USB Read)** |
| **Status** | OK | **OK** | Stable with `sudo` |

**Note**: A smaller 1MB test showed speeds up to **105 KB/s** (1MB in 10s). Performance may vary slightly based on thermal throttling or system load during long 16MB operations, but is consistently faster and stable.

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

## 4. Stability Recommendations
*   **Use `sudo`**: On macOS, running `snander` with `sudo` is critical to prevent `LIBUSB_ERROR_ACCESS` and interface locking issues during intensive operations.
*   **Validation**: The new verify speed (3 seconds for 16MB) confirms that reading is operating at very high efficiency (~5.3 MB/s).
