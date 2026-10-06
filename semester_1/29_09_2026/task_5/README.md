входы: wr_clk, {data, last}, wr_en, full
выходы: rd_clk, data_out, valid, rd_en, empty

использование:
daa_out <= mem[rd_addr]
valud <= rd_en
rd_addr <= rd_addr+1
wr_en = valid & !full
mem[wr_addr] <= data
if (wr_en) wr_addr += 1

если в брейкпоните увидим full и wr_en - то это ошибка

Задача: реализовать FIFO с двумя тактовыми сигналами
