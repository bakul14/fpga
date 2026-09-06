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
cd -
```

## Форматирование кода

В [VS Code](https://code.visualstudio.com/download?_exp_download=fb315fc982) ставим расширения **Verilog-HDL/SystemVerilog** от Masahiro Hiramori и **Clang-Format** от Xaver Hellauer. Теперь форматировать код на Verilog и C++ можно комбинацией клавиш `Shift+Option+F` на Mac OS и `Ctrl+Shift+I` на Linux.

В [Zed](https://zed.dev/download) ставим расширение **Verilog** от someone13574, C++ поддерживается из коробки. Теперь форматировать код можно комбинацией клавиш `Command+Shift+I` на Mac OS и `Ctrl+Shift+I` на Linux.

## Компиляция модуля, тестов и запуск нужной задачи
```bash
make task_1
```
> [!IMPORTANT]
> Одна задача включает набор .v-файлов логики на Verilog и .cpp-файлов модульных тестов на C++.
> Каждая задача располагается строго в отдельной папке, обязательно начинающейся с `task_`.
> Названия папок с задачами не должны повторяться, так как CMake ищет папку задачи рекурсивно внутри папок `semester_*/` и выбирает первую найденную.
> Примеры названий: `task_0`, `task_1`, `task_dz`, `task_67`.

## Автодополнение кода тестов на C++

При компиляции тестов задачи создается файл `build/<задача>/compile_commands.json` с флагами компиляции и файл `.clangd` рядом с исходниками, указывающий на этот файл. У каждой задачи своя база, так что они не мешают друг другу.

В Zed автодополнение работает сразу, в VS Code нужно расширение **clangd** от LLVM.

> [!IMPORTANT]
> Автодополнение появится только после первой сборки задачи через `make`, так как до неё нет `compile_commands.json` и исходников С++-модели от Verilator.

## Просмотр с помощью gtkview
```bash
make view
```

Если не нравится `gtkview` из-за слишком мелкого интерфейса, то в VSCode можно установить расширение **WaveTrace** для просмотра `.vcd`-файлов и использовать его вместо `gtkview`.

Отдельно и только на Mac OS можно установить `surfer` для просмотра `.vcd`-файлов (самый удобный):
```bash
brew install surfer
```
