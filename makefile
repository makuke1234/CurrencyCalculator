TARGET=CurCalc

SRC=src
OBJD=objd
OBJ=obj


CC=gcc
RESC=windres

CDEBCONST=-D _DEBUG
CCONST=-D NDEBUG

CDEFFLAGS=-municode -std=c2x -m64 -Wall -Wextra -Wpedantic -Wconversion -Wdouble-promotion -Wshadow -Wfree-nonheap-object -Wcast-align -Wunused -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wformat=2
CDEBFLAGS=-g -O0 $(CDEBCONST)
CFLAGS=-O3 -Wl,--strip-all,--build-id=none,--gc-sections -fno-ident $(CCONST)
LIB=-lgdi32

default: debug


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

$(OBJD)/%.rc.o: $(SRC)/%.rc $(OBJD)
	$(RESC) $< -o $@ $(CDEBCONST)

$(OBJD)/%.c.o: $(SRC)/%.c $(OBJD)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CDEBFLAGS)
$(OBJD)/%.$(ASMSUFFIX).o: $(SRC)/%.$(ASMSUFFIX) $(OBJD)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CDEBFLAGS)

$(OBJ)/%.rc.o: $(SRC)/%.rc $(OBJ)
	$(RESC) $< -o $@ $(CCONST)

$(OBJ)/%.c.o: $(SRC)/%.c $(OBJ)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CFLAGS)
$(OBJ)/%.$(ASMSUFFIX).o: $(SRC)/%.$(ASMSUFFIX) $(OBJ)
	$(CC) $< -c -o $@ $(CDEFFLAGS) $(CFLAGS)


debug: $(debug_obj)
	$(CC) $^ -o deb$(TARGET) $(CDEFFLAGS) $(CDEBFLAGS) $(LIB)


release: $(release_obj)
	$(CC) $^ -o $(TARGET) $(CDEFFLAGS) $(CFLAGS) $(LIB)

clean.o:
	IF EXIST $(OBJD) rd /s /q $(OBJD)
	IF EXIST $(OBJ) rd /s /q $(OBJ)

clean: clean.o
	del deb$(TARGET).exe
	del $(TARGET).exe
