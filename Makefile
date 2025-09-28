CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm
OBJ_EMULATE = emulate.o decDPR.o decDPI.o decBranch.o decSingleDTI.o 
OBJ_ASSEMBLE = assemble.o parse_functions.o assembler_global_vars.o 

OBJ_COMMON = helper_functions.o uint32DynamicList.o genericDynamicList.o

make: $(OBJ_EMULATE) $(OBJ_ASSEMBLE) $(OBJ_COMMON)
	$(CC) -o emulate $(OBJ_EMULATE) $(OBJ_COMMON) $(LDFLAGS)
	$(CC) -o assemble $(OBJ_ASSEMBLE) $(OBJ_COMMON) $(LDFLAGS)

emulate: $(OBJ_EMULATE) $(OBJ_COMMON)
	$(CC) -o emulate $(OBJ_EMULATE) $(OBJ_COMMON) $(LDFLAGS)

assemble: $(OBJ_ASSEMBLE) $(OBJ_COMMON)
	$(CC) -o assemble $(OBJ_ASSEMBLE) $(OBJ_COMMON) $(LDFLAGS)

emulate.o: emulate.c emulate.h decDPR.h decDPI.h decBranch.h decSingleDTI.h helper_functions.h uint32DynamicList.h
	$(CC) $(CFLAGS) -c emulate.c

helper_functions.o: helper_functions.c helper_functions.h uint32DynamicList.h  
	$(CC) $(CFLAGS) -c helper_functions.c

decDPR.o: decDPR.c decDPR.h helper_functions.h
	$(CC) $(CFLAGS) -c decDPR.c

decDPI.o: decDPI.c decDPI.h helper_functions.h
	$(CC) $(CFLAGS) -c decDPI.c

decBranch.o: decBranch.c decBranch.h helper_functions.h
	$(CC) $(CFLAGS) -c decBranch.c

decSingleDTI.o: decSingleDTI.c decSingleDTI.h helper_functions.h
	$(CC) $(CFLAGS) -c decSingleDTI.c

uint32DynamicList.o: uint32DynamicList.c uint32DynamicList.h
	$(CC) $(CFLAGS) -c uint32DynamicList.c

genericDynamicList.o: genericDynamicList.c genericDynamicList.h
	$(CC) $(CFLAGS) -c genericDynamicList.c

assemble.o: assemble.c assemble.h helper_functions.h assembler_global_vars.h
	$(CC) $(CFLAGS) -c assemble.c

assembler_global_vars.o: assembler_global_vars.c assembler_global_vars.h parse_functions.h uint32DynamicList.h
	$(CC) $(CFLAGS) -c assembler_global_vars.c

parse_functions.o: parse_functions.c parse_functions.h helper_functions.h assembler_global_vars.h
	$(CC) $(CFLAGS) -c parse_functions.c

clean:
	rm -f *.o emulate assemble
