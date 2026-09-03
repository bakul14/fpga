## Установка зависимостей

### На Mac OS (протестировано на Mac OS 27 beta 6)
```zsh
   xcode-select --install
   brew trust --formula chipsalliance/verible/verible
   brew install verilator gtkwave verible cmake clang-format googletest coreutils
```

### На Linux (протестировано на Ubuntu 22.04)
В убунте 22.04 в стандартном репозитории есть только старый Verilator 4.*, поэтому собираем из исходников версию поновее.
```bash
sudo apt update -y
sudo apt install -y build-essential cmake git gtkwave clang-format libgtest-dev autoconf flex bison help2man
git clone https://github.com/verilator/verilator.git ~/.local/src/verilator
cd ~/.local/src/verilator
git checkout v5.050
autoconf
./configure
make -j$(nproc)
sudo make install
wget -qO- https://github.com/chipsalliance/verible/releases/download/v0.0-4163-g6cce8f19/verible-v0.0-4163-g6cce8f19-linux-static-x86_64.tar.gz | sudo tar -xz --strip-components=2 -C /usr/local/bin
```

В VS Code ставим расширение **Verilog-HDL/SystemVerilog** от Masahiro Hiramori. Теперь форматировать код на C++ и Verilog можно комбинацией клавиш `Shift+Option+F` на Mac OS и `Ctrl+Shift+I` на Linux.

## Собрать и прогнать тесты нужного семинара:
```bash
make task_1
```

## Каждый тест пишет свою временную диаграмму, посмотреть её можно так:
```bash
gtkwave build/task_1/wave_increments_on_every_clock.vcd
```
