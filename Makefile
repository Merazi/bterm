install_dir=/usr/local/bin

make: config.h main.c
	gcc -O3 `pkg-config --cflags vte-2.91` main.c -o bterm `pkg-config --libs vte-2.91`

install: bterm
	cp -f bterm $(install_dir)
