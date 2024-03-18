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
asmrunkv:
	valgrind -s --leak-check=full ./asm ../assembler/kvadratka.txt bytecode.txt
asmrunfibo:
	valgrind -s --leak-check=full ./asm ../assembler/fibo.txt bytecode.txt


spu-main:
	g++  -g ../processor/processor.cpp ../stack/stack.cpp ../onegin/fileReader.c ../stack/stack.h ../stack/stack.hpp -o spu
spu-debug:
	g++ -fsanitize=address -DDEBUG -DSECURE -g ../stack/stack.cpp ../processor/processor.cpp ../onegin/fileReader.c ../stack/stack.hpp ../common/log.cpp -o spu


spurun:
	./spu bytecode.txt
spurunval:
	valgrind --leak-check=full ./spu bytecode.txt


treeTest-m:
	g++ ../BinaryTree/BinaryTree.cpp ../BinaryTree/test.cpp -o Ttest


akin-main:
	g++ -g ../stack/stack.hpp ../stack/stack.cpp ../akinator/akinator.cpp ../akinator/akinate.cpp ../akinator/workWithDescription.cpp ../BinaryTree/BinaryTree.cpp ../akinator/dataBaseRead.cpp ../onegin/fileReader.c -o akinator
akin-debug:
	g++ -g -DDEBUG ../common/log.cpp ../stack/stack.hpp ../stack/stack.cpp ../akinator/akinator.cpp ../akinator/akinate.cpp ../akinator/workWithDescription.cpp ../BinaryTree/BinaryTree.cpp ../akinator/dataBaseRead.cpp ../onegin/fileReader.c -o akinator

akinrun:
	./akinator
akinrunval:
	valgrind -s --track-origins=yes --leak-check=full ./akinator akinatorLog.log


akin-dataRead:
	g++ ../akinator/dataBaseRead.cpp ../onegin/fileReader.c ../BinaryTree/BinaryTree.cpp