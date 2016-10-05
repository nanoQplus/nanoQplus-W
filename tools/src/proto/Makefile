BIN=$(NOS_HOME)/tools/proto.exe
SRC=proto.c
OBJ=${SRC:.c=.o}
LIB=

all : $(BIN)

$(BIN) : $(OBJ)
	gcc -o $@ $(OBJ) $(LIB)


$(OBJ) : $(SRC)
	gcc -c $<

clean :
	-rm -f $(BIN) $(OBJ)
