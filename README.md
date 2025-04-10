# Still under development

# 🖥️ KernelSpy – Terminal System Monitor  

**KernelSpy** is a **lightweight, terminal-based system monitor** for Linux, built in **C with ncurses**. It provides **real-time insights** into CPU usage, memory, disk, and network activity — all in a sleek and stylish CLI interface. Perfect for performance monitoring, debugging, and power users who prefer a **fast, efficient, and resource-friendly** alternative to GUI-heavy system monitors.  

---

## 🚀 Features  
✔ **Live CPU, Memory, Disk, and Network usage tracking**  
✔ **Color-coded usage bars (green/yellow/red) for clear thresholds**  
✔ **Polished ncurses UI with borders, centered menus, and footer clock**  
✔ **Lightweight and perfect for Arch Linux (and others)**  
✔ **Open-source & customizable**  

---

## 🛠️ Installation & Usage  
```bash
# 1️⃣ Clone the repository
git clone https://github.com/YOUR-USERNAME/KernelSpy.git
cd KernelSpy

# 2️⃣ Build it
sudo make

# 3️⃣ Run it
./kernelspy

# 4️⃣ Optional: Clean build files
sudo make clean
```

---

## 🧠 Interface Overview  
When you launch KernelSpy, you'll see a **ncurses-powered menu** with options:

```bash
[ CPU ]      ← Per-core usage with live frequency
[ Network ]  ← Download/Upload stats in KB/s
[ Memory ]   ← Total, used, cached, buffers + usage bar
[ Disk ]     ← Mount point usage, type (SSD/HDD), I/O activity
```

All modules support:
- `'q'` to return to menu
- Live updating stats

> 🧊 Temperature module was removed due to unreliable sensor data.

---

## 🤝 Contributions Welcome!
```bash
# 1️⃣ Fork the repo  
# 2️⃣ Create a new branch  
# 3️⃣ Commit your changes  
# 4️⃣ Push to your branch & submit a pull request
```

---

## 📦 AUR Packaging (Coming Soon)
KernelSpy will be available on the AUR so you can install it via `yay`:
```bash
yay -S kernelspy
```

---

## 💻 License  
KernelSpy is open-source under the MIT License. Contributions and feedback are always welcome!
