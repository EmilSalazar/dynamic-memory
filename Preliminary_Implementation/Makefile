myFile	:
	gcc -pthread -Wall main.c my_malloc.c -o myFile

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./myFile

clean:
	rm -v myFile
