SHELL=cmd

TARGET=CurCalc

BIN=bin
SRC=src
OBJD=objd
OBJ=obj

export C_INCLUDE_PATH=./include
export LIBRARY_PATH=./lib


CC=gcc
RESC=windres

CDEBCONST=-D _DEBUG
CCONST=-D NDEBUG

CDEFFLAGS=-municode -std=c2x -m64 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion -Wshadow -Wfree-nonheap-object -Wcast-align -Wunused -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wformat=2
CDEBFLAGS=-g -O0 $(CDEBCONST)
CFLAGS=-O3 -Wl,--strip-all,--build-id=none,--gc-sections -fno-ident $(CCONST) -mwindows
LIB=-lgdi32 -lcurl

default: clean runDeb


$(BIN):
	mkdir $(BIN)

$(OBJD):
	mkdir $(OBJD)
$(OBJ):
	mkdir $(OBJ)

ASMSUFFIX=S

C_SRCS=$(wildcard $(SRC)/*.c)
ASM_SRCS=$(wildcard $(SRC)/*.$(ASMSUFFIX))
RC_SRCS=$(wildcard $(SRC)/*.rc)

allobj=$(C_SRCS:%.c=%.c.o)
allobj+=$(ASM_SRCS:%.$(ASMSUFFIX)=%.$(ASMSUFFIX).o)
allobj+=$(RC_SRCS:%.rc=%.rc.o)

debug_obj:=$(allobj:$(SRC)/%=$(OBJD)/%)
release_obj:=$(allobj:$(SRC)/%=$(OBJ)/%)

$(OBJD)/%.rc.o: $(SRC)/%.rc $(OBJD) $(BIN)
	$(RESC) $< -o $@ $(CDEBCONST)

$(OBJD)/%.c.o: $(SRC)/%.c $(OBJD) $(BIN)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CDEBFLAGS)

$(OBJD)/%.$(ASMSUFFIX).o: $(SRC)/%.$(ASMSUFFIX) $(OBJD) $(BIN)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CDEBFLAGS)



$(OBJ)/%.rc.o: $(SRC)/%.rc $(OBJ) $(BIN)
	$(RESC) $< -o $@ $(CCONST)

$(OBJ)/%.c.o: $(SRC)/%.c $(OBJ) $(BIN)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CFLAGS) $(BIN)

$(OBJ)/%.$(ASMSUFFIX).o: $(SRC)/%.$(ASMSUFFIX) $(OBJ)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CFLAGS)



debug: $(debug_obj)
	$(CC) $^ -o $(BIN)/deb$(TARGET) $(CDEFFLAGS) $(CDEBFLAGS) $(LIB)


release: $(release_obj)
	$(CC) $^ -o $(BIN)/$(TARGET) $(CDEFFLAGS) $(CFLAGS) $(LIB)

clean.o:
	IF EXIST $(OBJD) rd /s /q $(OBJD)
	IF EXIST $(OBJ) rd /s /q $(OBJ)

clean: clean.o $(BIN)
	del $(BIN)\deb$(TARGET).*
	del $(BIN)\$(TARGET).*

runDeb: debug
	$(BIN)/deb$(TARGET)

runRel: release
	$(BIN)/$(TARGET)
