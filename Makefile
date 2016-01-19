CROSS=arm-none-linux-gnueabi-

all: armcomtest 
#x86comtest

armcomtest: Centre_Controller.c DELU_Heat_Meter.c Elect97.c WaterMeter.c Comm_with_server.c queue.c tasks_up.c gprs.c gprs_mana.c publicfunc.c misc.c commap.c xml.c CommandDeal.c
	$(CROSS)gcc -Wall -o armcomtest -g Centre_Controller.c DELU_Heat_Meter.c Elect97.c WaterMeter.c Comm_with_server.c queue.c tasks_up.c gprs.c gprs_mana.c publicfunc.c misc.c commap.c xml.c CommandDeal.c\
	 -lpthread -I ./include/libxml2 -I ./openssl/include -L ./arm_lib -lxml2  -L ./sqlite/lib -L ./openssl/lib -lsqlite3 -lssl -lcrypto --static
#-L ./arm_lib  -lxml2
#x86comtest: Centre_Controller.c DELU_Heat_Meter.c
#gcc -o x86comtest -g comtest.c DELU_Heat_Meter.c

clean:
	@rm -vf armcomtest x86comtest *.o *~
