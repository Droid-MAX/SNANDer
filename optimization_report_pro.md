# SNANDer CH347 Optimization Report (macOS)

## Scope
This document consolidates the optimization work and measured results for SNANDer on macOS using CH347F. It focuses on SPI NOR (W25Q128BV, 16MB) and SPI NAND (W25N01GV, 128MB).

## Environment
- OS: macOS (libusb/darwin)
- Programmer: CH347F (USB)
- SPI NOR: Winbond W25Q128BV (16MB)
- SPI NAND: Winbond W25N01GV (128MB)
- Test files: `test_16M_random.bin` (16,777,216 bytes), `test_128M_random.bin` (134,217,728 bytes)
- Notes: `sudo` recommended on macOS to avoid `LIBUSB_ERROR_ACCESS`.

## Optimization Timeline and Results (SPI NOR)

### Baseline (pre-optimization)
- Command: `./releases/darwin-arm64/snander -w test_data.bin`
- Write: 251s
- Status: OK

### Optimization 1: Write bundling + unprotect reduction
Changes:
- Bundle opcode + address + 256B page into one transfer in `snor_write`.
- Remove redundant `snor_unprotect` inside write loop.
- Fix `libusb` type mismatch (macOS hang fix).

Result:
- Command: `sudo ./releases/darwin-arm64/snander -w test_16M.bin -v`
- Write: 215s
- Verify: 3s
- Status: OK

### Optimization 2: Adaptive WIP polling + per-page wait correctness
Changes:
- Adaptive WIP polling in `snor_wait_ready`.
- Only check `WIP` for ready.
- Ensure per-page wait after program (correctness).

Results (latest code):
- Command: `sudo -E ./build/snander -w test_16M_random.bin -v`
  - Write: 78s
  - Verify: 3s
  - Status: OK
- Command: `SNANDER_TIMING=1 sudo -E ./build/snander -t -w test_16M_random.bin -v`
  - Write: 84s
  - Timing: usb/command 17.340s, wait 51.571s (poll 49.722s, sleep 1.844s), other 15.069s
  - Status: OK

### SPI Speed Control
Change:
- Added `-S` to select SPI speed (60M/30M/15M/10M/5M/2M).

Results:
- Command: `sudo -E ./build/snander -w test_16M_random.bin -S 30M -v`
  - Write: 85s
  - Verify: 5s
  - Status: OK

Observation: write time dominated by page program + WIP polling; SPI clock impacts read more than total write time.

## SPI NAND Results (W25N01GV, 128MB)

Command:
```
sudo -E ./build/snander -w test_128M_random.bin -v -d
```

Results:
- Write: 317s (~0.40 MB/s)
- Verify read: 103s (~1.24 MB/s)
- Status: OK

Optimization note:
- Added adaptive OIP polling delays in NAND load/erase/write loops to reduce host polling overhead on macOS.
- Follow-up testing recommended to quantify gains.

## Summary of Final Performance (SPI NOR)
- Initial: 251s write
- Intermediate: 215s write
- Final: 78s write, 3s verify
- Improvement: ~3.1x write speed increase vs baseline

## Commands (Reference)
```
sudo -E ./build/snander -i
sudo -E ./build/snander -e
sudo -E ./build/snander -w test_16M_random.bin -v
SNANDER_TIMING=1 sudo -E ./build/snander -t -w test_16M_random.bin -v
sudo -E ./build/snander -w test_16M_random.bin -S 30M -v
sudo -E ./build/snander -w test_128M_random.bin -v -d
```
