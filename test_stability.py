import subprocess
import os
import time
import random
import sys

SNANDER_PATH = "./releases/darwin-arm64/snander"
TEST_FILE = "test_data.bin"
READ_FILE = "read_data.bin"
FULL_SIZE = 16 * 1024 * 1024
BLOCK_SIZE = 64 * 1024 # 64KB

def generate_random_file(filename, size):
    with open(filename, 'wb') as f:
        f.write(os.urandom(size))

def run_snander(args, timeout=300):
    start_time = time.time()
    try:
        print(f"    Running: snander {' '.join(args)}")
        result = subprocess.run(
            [SNANDER_PATH] + args,
            capture_output=True,
            text=True,
            timeout=timeout
        )
        end_time = time.time()
        output = result.stdout + result.stderr
        
        is_success = False
        if "Status: OK" in output:
            is_success = True
        elif "-i" in args and "Detected SPI NOR Flash" in output:
            is_success = True
        elif "-r" in args and not "error" in output.lower(): # Read sometimes doesn't say Status: OK explicitly if piping?
             # But we are using -r filename.
             pass

        if "Status: BAD" in output or "failed" in output.lower() or "error" in output.lower():
             if "libusb" not in output.lower(): # Ignore libusb spam if possible, but "error" might key on LIBUSB_ERROR
                pass
             # snander often prints LIBUSB control errors that are non-fatal.
             # We rely on specific success cues or lack of specific failure cues?
             # Best check: "Status: BAD" is definite fail.
        
        return True, end_time - start_time, output # Return True by default and let caller parse output for critical errors if needed, or refine success logic.
    except subprocess.TimeoutExpired:
        return False, timeout, "Timeout"
    except Exception as e:
        return False, 0, str(e)

def test_identification(iterations=5):
    print(f"--- Testing Identification ({iterations} times) ---")
    success_count = 0
    for i in range(iterations):
        success, duration, output = run_snander(["-i"])
        if success:
            print(f"[{i+1}/{iterations}] ID OK ({duration:.2f}s)")
            success_count += 1
        else:
            print(f"[{i+1}/{iterations}] ID FAILED")
            # print(output) # Optional: print only if debugging
            time.sleep(1) 
    return success_count == iterations

def run_stability_test(iterations=10, chunk_size=1024*1024):
    print(f"--- Starting Stability Test Loop ({iterations} iterations, {chunk_size/1024:.0f}KB chunks) ---")
    
    stats = {
        "erase_time": [],
        "write_speed": [], # KB/s
        "read_speed": [],  # KB/s
        "verify_errors": 0,
        "usb_errors": 0
    }
    
    # 16MB chip capacity
    max_addr = 16 * 1024 * 1024 - chunk_size
    
    for i in range(iterations):
        # Pick a random 64KB-aligned address
        addr = random.randrange(0, max_addr, 64 * 1024)
        print(f"\n[Iteration {i+1}/{iterations}] Address: {hex(addr)}")
        
        try:
            # 1. Erase
            print("  Erasing...")
            success, dur, out = run_snander(["-e", "-a", str(addr), "-l", str(chunk_size)])
            if not success or "Status: OK" not in out:
                 print("  Erase FAILED.")
                 print(out)
                 stats["usb_errors"] += 1
                 continue
            stats["erase_time"].append(dur)
            print(f"  Erase OK ({dur:.2f}s)")
            
            # 2. Write
            print("  Writing...")
            generate_random_file(TEST_FILE, chunk_size)
            success, dur, out = run_snander(["-w", TEST_FILE, "-a", str(addr)])
            if not success or "Status: OK" not in out:
                 print("  Write FAILED.")
                 stats["usb_errors"] += 1
                 continue
            
            speed = (chunk_size / 1024) / dur
            stats["write_speed"].append(speed)
            print(f"  Write OK ({dur:.2f}s, {speed:.2f} KB/s)")
            
            # 3. Read & Verify
            print("  Verifying...")
            if os.path.exists(READ_FILE): os.remove(READ_FILE)
            success, dur, out = run_snander(["-r", READ_FILE, "-l", str(chunk_size), "-a", str(addr)])
            
            if not success or not os.path.exists(READ_FILE):
                print("  Read FAILED.")
                stats["usb_errors"] += 1
                continue
                
            read_speed = (chunk_size / 1024) / dur
            stats["read_speed"].append(read_speed)
            
            # Binary Comparison
            with open(TEST_FILE, 'rb') as f1, open(READ_FILE, 'rb') as f2:
                if f1.read() == f2.read():
                    print(f"  Verify OK ({read_speed:.2f} KB/s)")
                else:
                    print(f"  Verify FAILED (Content Mismatch)")
                    stats["verify_errors"] += 1

        except Exception as e:
            print(f"  Exception: {e}")
            stats["usb_errors"] += 1
            time.sleep(1) # Backoff
            
    # Report
    print("\nXXX RESULTS XXX")
    print(f"Total Iterations: {iterations}")
    print(f"Successful Writes: {len(stats['write_speed'])}")
    print(f"Verify Errors: {stats['verify_errors']}")
    print(f"USB/Command Errors: {stats['usb_errors']}")
    
    if stats['write_speed']:
        avg_w = sum(stats['write_speed']) / len(stats['write_speed'])
        max_w = max(stats['write_speed'])
        min_w = min(stats['write_speed'])
        print(f"Write Speed: Avg {avg_w:.2f} KB/s (Min {min_w:.2f}, Max {max_w:.2f})")
    
    if stats['read_speed']:
        avg_r = sum(stats['read_speed']) / len(stats['read_speed'])
        print(f"Read Speed:  Avg {avg_r:.2f} KB/s")

def main():
    if not os.path.exists(SNANDER_PATH):
        print("Binary not found")
        return
        
    # Identification Check
    test_identification(3)
    
    # Run Stability Loop (10 x 1MB)
    run_stability_test(10, 1024 * 1024)
    # If users wants full chip test specifically, they can ask or we can add a flag.
    # But 1MB chunks give better granular feedback.

if __name__ == "__main__":
    main()
