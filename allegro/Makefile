CC = gcc
cflag = -fPIC
debug = -Wall -Wextra -fsanitize=undefined -fsanitize=address
lin = -L ${PWD}/lib/ -l allegro -l allegro_font -l allegro_image -l allegro_ttf
#lin = -L ${PWD}/lib -lallegro_main
inc = -I ${PWD}/include/allegro5
out = -o main.out
target = ./main.c


ALL : ${target}
	LD_LIBRARY_PATH=${PWD}/lib ${CC} ${target} ${cflag} ${inc} ${lin} ${out}
debug : 
	$LD_LIBRARY_PATH=${PWD}/lib ${CC} ${target} ${cflag} ${lin} ${inc} ${debug} ${out}
clean : main.out
	rm -rf main.out
