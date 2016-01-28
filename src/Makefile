CROSS			=arm-none-linux-gnueabi-
TARGET			=arm9test
SRC				=Centre_Controller.c DELU_Heat_Meter.c Elect97.c WaterMeter.c Comm_with_server.c queue.c tasks_up.c gprs.c gprs_mana.c publicfunc.c misc.c commap.c xml.c CommandDeal.c
CC				=$(CROSS)gcc
INCLUDEPATH		=-I ../include -I ../include/libxml2
CFLAGS			=-g -Wall -o $(TARGET)
LIBDIR			=-L ../lib
LIBS			=-lxml2  -lsqlite3 -lssl -lcrypto -lpthread

all: $(TARGET)

arm9test: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDEPATH) $(LIBDIR) $(LIBS)

clean:
	@rm -vf $(TARGET) *.o *~
