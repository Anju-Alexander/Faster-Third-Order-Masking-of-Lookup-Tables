all: output
		./output

output: main.o aes.o aes_htable_PRG.o aes_Shares_prg.o present.o present_shares_prg.o present_htable_PRG.o bitslice.o shares.o common.o share.o aes_htable_RP.o
		gcc main.o aes.o aes_htable_PRG.o aes_Shares_prg.o present.o present_shares_prg.o present_htable_PRG.o bitslice.o shares.o common.o share.o aes_htable_RP.o -o output

main.o: main.c
		gcc -c main.c

aes.o: AES/aes.c
		gcc -c AES/aes.c

aes_htable_PRG.o: AES/aes_htable_PRG.c
		gcc -c AES/aes_htable_PRG.c

aes_Shares_prg.o: AES/aes_Shares_prg.c
		gcc -c AES/aes_Shares_prg.c

aes_htable_RP.o: AES/aes_htable_RP.c
		gcc -c AES/aes_htable_RP.c

present.o: PRESENT/present.c
		gcc -c PRESENT/present.c

present_shares_prg.o: PRESENT/present_shares_prg.c
		gcc -c PRESENT/present_shares_prg.c

present_htable_PRG.o: PRESENT/present_htable_PRG.c
		gcc -c PRESENT/present_htable_PRG.c

bitslice.o: BITSLICE/bitslice.c
		gcc -c BITSLICE/bitslice.c

shares.o: BITSLICE/shares.c
		gcc -c BITSLICE/shares.c
		
common.o: Util/common.c
		gcc -c Util/common.c

share.o: Util/share.c
		gcc -c Util/share.c

clean_windows:
		del /F /Q  output.exe *.o
clean_linux:
		rm *.o output
