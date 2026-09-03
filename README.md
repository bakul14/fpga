## Установка зависимостей на Mac OS (протестировано на Mac OS 27 beta 6)

Предполагается, что Homebrew и VS Code уже установлены.

1. Apple CLT (command line tools: clang, make, git)
```zsh
   xcode-select --install
```

2. Синтезатор, визуализатор и симейк
```zsh
   brew trust --formula chipsalliance/verible/verible
   brew install verilator gtkwave verible cmake clang-format googletest coreutils
```

3. В VS Code ставим расширение **Verilog-HDL/SystemVerilog** от Masahiro Hiramori.

## Проверка

Собрать и прогнать тесты нужного семинара:
```zsh
make task_1
make clean
```

Каждый тест пишет свою временную диаграмму, посмотреть её можно так:
```zsh
gtkwave build/task_0/wave_increments_on_every_clock.vcd
```

Форматировать код можно комбинацией клавиш `Shift+Option+F`.
