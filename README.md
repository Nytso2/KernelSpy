# 🖥️ KernelSpy – Terminal System Monitor  

**KernelSpy** is a **lightweight, terminal-based system monitor** for Linux, built in **C with ncurses**. It provides **real-time insights** into CPU usage, memory, disk, network activity, and system temperature, all in a sleek and minimal CLI interface. Perfect for performance monitoring, debugging, and power users who prefer a **fast, efficient, and resource-friendly** alternative to GUI-heavy system monitors.  

---

## 🚀 Features  
✔ **Live CPU, Memory, Disk, and Network usage tracking**  
✔ **Dynamic color-coded stats for easy monitoring**  
✔ **Lightweight, fast, and optimized for the command line**  
✔ **Perfect for Arch Linux, Debian, and other distros**  
✔ **Open-source & customizable**  

---

## 🛠️ Installation  
```bash
# 1️⃣ Clone the repository
git clone https://github.com/YOUR-USERNAME/KernelSpy.git
cd KernelSpy

# 2️⃣ Compile the source code
gcc sys_monitor.c -o KernelSpy -lncurses

# 3️⃣ Run KernelSpy
./KernelSpy


# Once running, KernelSpy will display real-time stats for:
# ✅ CPU Usage  
# ✅ Memory Usage  
# ✅ Disk Usage  
# ✅ Network Traffic (Download & Upload in KB/s)  
# ✅ CPU Temperature  

# Press Ctrl + C to exit.  

# Test CPU & Memory stress
stress-ng --vm 2 --vm-bytes 90% --vm-method all --timeout 5s

# Test Network traffic detection
wget http://speedtest.tele2.net/1GB.zip
