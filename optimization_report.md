# SNANDer Optimization & Performance Report (macOS)

## 1. Overview
This document summarizes recent optimizations, usage, and measured results for SNANDer on macOS with CH347F.

## 2. Environment
- Programmer: CH347F (USB)
- OS: macOS (libusb/darwin)
- SPI NOR: W25Q128BV (16MB)
- SPI NAND: W25N01GV (128MB)
- Test files: `test_16M_random.bin` (16,777,216 bytes), `test_128M_random.bin` (134,217,728 bytes)
- Note: non-sudo runs may show libusb access warnings and can affect stability.

## 3. Optimization Summary
- Packet bundling for NOR page writes (opcode + address + data in one transfer).
- Reduced NOR write loop overhead (avoid redundant unprotect).
- Adaptive WIP polling for NOR with timing breakdown output.
- Adaptive OIP polling delays for NAND load/erase/write loops.
- SPI speed selection via `-S`.
- libusb type fix on macOS to avoid hangs.

## 4. Usage

Quick start:
```
sudo -E ./build/snander -i
sudo -E ./build/snander -e
sudo -E ./build/snander -w test_16M_random.bin -v
```

Common options:
- `-i` probe device and print flash ID
- `-e` erase (full chip unless combined with `-a`/`-l`)
- `-w <file>` write from file
- `-r <file>` read to file
- `-v` verify after write
- `-S <speed>` set SPI speed (`60M`/`30M`/`15M`/`10M`/`5M`/`2M`)
- `-t` enable SPI NOR timing breakdown
- `-d` disable NAND internal ECC (for raw access)

Examples:
```
sudo -E ./build/snander -S 60M -w test_16M_random.bin -v
SNANDER_TIMING=1 sudo -E ./build/snander -t -w test_16M_random.bin -v
sudo -E ./build/snander -w test_128M_random.bin -v -d
```

## 5. Results (SPI NOR, W25Q128BV 16MB)

### 5.1 Baseline vs Optimized
| Metric | Initial | Optimized (latest) | Improvement |
| :--- | :--- | :--- | :--- |
| **Write Time** | 251s | **78s** | **-69%** |
| **Write Speed** | ~66.8 KB/s | **~205 KB/s** | **~3.1x** |
| **Verify Time** | N/A | **3s** | **5.3 MB/s (USB read limit)** |
| **Status** | OK | **OK** | Stable with `sudo` |

**Note**: An earlier optimized pass reached **215s** (from 251s).

### 5.2 SPI Speed Impact
| Metric | Default (60M) | 30M | Notes |
| :--- | :--- | :--- | :--- |
| **Write Time** | **78s** | **85s** | Write time changes little with SPI speed |
| **Verify Time** | **3s** | **5s** | Read slower at 30M |
| **Status** | OK | OK | Verified OK |

Timing breakdown (60M):
```
Timing: usb/command 17.340s, wait 51.571s (poll 49.722s, sleep 1.844s, other 0.005s), other 15.069s
```

## 6. Results (SPI NAND, W25N01GV 128MB)

Command:
```
sudo -E ./build/snander -w test_128M_random.bin -v -d
```

Results:
- Write: 317s (~0.40 MB/s)
- Verify read: 103s (~1.24 MB/s)
- Status: OK

## 7. Stability Notes
- Use `sudo` on macOS to avoid `LIBUSB_ERROR_ACCESS`.
- Write time is dominated by page program/WIP polling; SPI clock affects read more than total write time.
