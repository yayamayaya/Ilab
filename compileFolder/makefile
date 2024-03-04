asm-main:
	g++ ../assembler/assembler.cpp ../assembler/fileRead.cpp ../common/log.cpp -o asm
asm-debug:
	g++ -DDEBUG ../assembler/assembler.cpp ../assembler/fileRead.cpp ../common/log.cpp -o asm

asmrun:
	./asm code.txt bytecode.txt logFile.log
asmrunval:
	valgrind ./asm code.txt bytecode.txt


spu-main:
	g++ ../processor/processor.c ../onegin/fileReader.c ../stack/stack.c -o spu
spu-debug:
	g++ -DDEBUG ../processor/processor.c ../onegin/fileReader.c ../stack/stack.c ../common/log.cpp -o spu


spurun:
	./spu bytecode.txt
spurunval:
	valgrind --leak-check=full ./spu bytecode.txt


treeTest-m:
	g++ ../akinator/BinaryTree.cpp ../akinator/test.cpp -o Ttest