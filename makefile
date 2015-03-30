CC = gcc
CFLAGS = -w -g
LDFLAGS = -liw -lpthread
all : test 

test : main.c wireless_manager.c handover_manager.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(INCLUDE) -DCONFIG_LIBNL20
	
clean :
	@rm -rf *.o test
