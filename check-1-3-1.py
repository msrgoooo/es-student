import serial
import time
import sys

PORT = "COM7"          # ← замените на ваш порт
BAUD = 115200
DURATION = 10
LOG_FILE = "device-1-3-1.log"

def main():
    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except serial.SerialException as e:
        print(f"Не удалось открыть порт {PORT}: {e}")
        sys.exit(1)

    print(f"Слушаю {PORT} в течение {DURATION} с...")

    start = time.time()
    lines = []

    with open(LOG_FILE, "w", encoding="utf-8") as log:
        while time.time() - start < DURATION:
            raw = ser.readline()
            if not raw:
                continue
            line = raw.decode("utf-8", errors="replace").strip()
            if line:
                print(line)
                log.write(line + "\n")
                log.flush()
                lines.append(line)

    ser.close()
    print(f"\nПринято строк: {len(lines)}")
    print(f"Лог записан в {LOG_FILE}")

if __name__ == "__main__":
    main()