helom: funcs_processed6.c data.c data.h utils.c utils.h
	gcc   data.c  utils.c funcs_processed6.c -lm -O3 -std=c11 -march=native -D_SVID_SOURCE -o ToyLang_optimized3
	gcc   data.c  utils.c funcs_processed6.c -lm -O2 -std=c11 -march=native -D_SVID_SOURCE -o ToyLang_optimized2
	gcc   data.c  utils.c funcs_processed6.c -lm -Og -ggdb3 -std=c11 -march=native -D_SVID_SOURCE -o ToyLang_debug

