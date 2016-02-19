CROSS			=arm-none-linux-gnueabi-
TARGET			=arm9test
BIN				=../bin/$(TARGET)
SRC				=Centre_Controller.c DELU_Heat_Meter.c Elect97.c WaterMeter.c Comm_with_server.c queue.c tasks_up.c gprs.c gprs_mana.c publicfunc.c misc.c commap.c xml.c CommandDeal.c
INCLUDEPATH		=-I ../include -I ../include/libxml2
CFLAGS			=-g -Wall -o $(BIN)
LIBDIR			=-L ../lib
LIBS			=-lxml2  -lsqlite3 -lssl -lcrypto -lpthread
NFSROOT			=/nfsroot
CP				=cp
CC				=$(CROSS)gcc
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDEPATH) $(LIBDIR) $(LIBS)
	$(CP) $(BIN) $(NFSROOT)
clean:
	@rm -vf $(BIN) *.o *~
