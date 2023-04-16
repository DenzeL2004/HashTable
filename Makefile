all: mkdirectory run

FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations 		\
		-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion	\
		-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers		\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel  	\
		-Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE


run: 	 obj/generals.o obj/log_errors.o obj/word_reader.o obj/list.o obj/hash_table.o obj/hash_func.o obj/test.o obj/main.o
	g++  obj/generals.o obj/log_errors.o obj/word_reader.o obj/list.o obj/hash_table.o obj/hash_func.o obj/test.o obj/main.o -o run


obj/main.o: main.cpp
		g++ main.cpp -c -o obj/main.o $(FLAGS)


obj/test.o: test_hash_table/test_hash_table.cpp test_hash_table/test_hash_table.h test_hash_table/config.h 
		g++ test_hash_table/test_hash_table.cpp -c -o obj/test.o $(FLAGS)

obj/word_reader.o:  src/word_reader/word_reader.cpp src/word_reader/word_reader.h
				g++ src/word_reader/word_reader.cpp -c -o obj/word_reader.o $(FLAGS)


obj/log_errors.o: src/log_info/log_errors.h src/log_info/log_errors.cpp
			  g++ src/log_info/log_errors.cpp -c -o obj/log_errors.o $(FLAGS)

obj/generals.o: src/generals_func/generals.cpp src/generals_func/generals.h
			g++ src/generals_func/generals.cpp -c -o obj/generals.o $(FLAGS)


obj/list.o: src/list/list.cpp src/list/list.h src/list/config_list.h
		g++ src/list/list.cpp -c -o obj/list.o $(FLAGS)


obj/hash_table.o: src/hash_table/hash_table.cpp src/hash_table/hash_table.h 
			  g++ src/hash_table/hash_table.cpp -c -o obj/hash_table.o $(FLAGS)

obj/hash_func.o: src/hash_table/hash_functions.cpp src/hash_table/hash_functions.h 
			 g++ src/hash_table/hash_functions.cpp -c -o obj/hash_func.o $(FLAGS)



.PHONY: cleanup mkdirectory

mkdirectory:
	mkdir -p obj;
	mkdir -p temp;
	

cleanup:
	rm obj/*.o