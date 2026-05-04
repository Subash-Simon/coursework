# Power Quality Waveform Analyser

## 👤 Student Details
Name: Subash Pun  
Student ID: 24049483

---

## 📌 Project Description
This project is a C program that analyses a 3-phase power quality dataset from a CSV file.  
It calculates important electrical parameters and detects anomalies in the waveform.

---

## ⚙️ Features
- Reads CSV file with 1000 samples
- Calculates RMS voltage for Phase A, B, and C
- Calculates peak-to-peak voltage
- Calculates DC offset
- Detects clipping events (|voltage| ≥ 324.9 V)
- Computes standard deviation
- Analyses frequency, power factor, THD, and current
- Checks voltage compliance (207–253 V range)
- Generates a report file (`results.txt`)

---

## 📁 Files Included
- `main.c` → Main program logic
- `waveform.c` → Voltage calculations
- `waveform.h` → Structs and definitions
- `io.c` → CSV reading and report writing
- `io.h` → Function declarations
- `CMakeLists.txt` → Build configuration

---

## ▶️ How to Run

### Using Terminal:
```bash
cc main.c waveform.c io.c -o minicoursework -lm
./minicoursework power_quality_log.csv