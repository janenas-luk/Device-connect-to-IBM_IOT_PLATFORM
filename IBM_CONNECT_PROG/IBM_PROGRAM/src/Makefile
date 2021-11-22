#This is IBM_PROGRAM Makefile
BIN:= IBM_PROGRAM
LDFLAGS:= -liotp-as-device -lblobmsg_json -lubox -lubus
CPPFLAGS:= -I$(CURDIR)
SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN):$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)