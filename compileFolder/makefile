asm-main:
	g++ ../assembler/assembler.cpp ../assembler/fileRead.cpp ../common/log.cpp -o asm
asm-debug:
	g++ -g -DDEBUG ../assembler/assembler.cpp ../assembler/fileRead.cpp ../common/log.cpp -o asm

asmrun:
	./asm code.txt bytecode.txt logFile.log
asmrunval:
	valgrind ./asm code.txt bytecode.txt
asmrunfact:
	./asm ../assembler/factorial.txt bytecode.txt logFile.log


spu-main:
	g++  -g ../processor/processor.cpp ../onegin/fileReader.c ../stack/stack.h ../stack/stack.hpp -o spu
spu-debug:
	g++ -fsanitize=address -DDEBUG -DSECURE -g ../processor/processor.cpp ../onegin/fileReader.c ../stack/stack.h ../common/log.cpp -o spu


spurun:
	./spu bytecode.txt
spurunval:
	valgrind --leak-check=full ./spu bytecode.txt


treeTest-m:
	g++ ../BinaryTree/BinaryTree.cpp ../BinaryTree/test.cpp -o Ttest


akin-main:
	g++ ../akinator/akinator.cpp ../BinaryTree/BinaryTree.cpp ../akinator/dataBaseRead.cpp ../onegin/fileReader.c -o akinator

akinrun:
	./akinator
akinrunval:
	valgrind ./akinator


akin-dataRead:
	g++ ../akinator/dataBaseRead.cpp ../onegin/fileReader.c ../BinaryTree/BinaryTree.cpp