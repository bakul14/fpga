## Установка зависимостей

### На Mac OS (протестировано на Mac OS 27 beta 6)
```zsh
xcode-select --install
bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
brew trust --formula chipsalliance/verible/verible
brew install --HEAD randomplum/gtkwave/gtkwave
brew install verilator verible cmake clang-format googletest coreutils
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

## Форматирование кода

В [VS Code](https://code.visualstudio.com/download?_exp_download=fb315fc982) ставим расширения **Verilog-HDL/SystemVerilog** от Masahiro Hiramori и **Clang-Format** от Xaver Hellauer. Теперь форматировать код на Verilog и C++ можно комбинацией клавиш `Shift+Option+F` на Mac OS и `Ctrl+Shift+I` на Linux.

## Компиляция модуля, тестов и запуск для нужного семинара
```bash
make task_1
```
> [!IMPORTANT]
> Одна задача включает набор .v-файлов логики на Verilog и .cpp-файлов модульных тестов на C++.
> Каждая задача располагается в отдельной папке, обязательно начинающейся с `task_`.
> Повторений названий не должно быть, так как cmake будет искать папку задачи рекурсивно внутри папок `semester_*/`.
> Примеры названий: `task_0`, `task_1`, `task_dz`, `task_67`.

## Просмотр с помощью gtkview
```bash
make view
```

Если не нравится `gtkview` из-за слишком мелкого интерфейса, то в VSCode можно установить расширение **WaveTrace** для просмотра `.vcd`-файлов и использовать его вместо `gtkview`.
