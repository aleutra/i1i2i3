CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lm -lpthread -lsox
OBJS = menu.o record.o fax.o phone_test.o
TARGET = i3

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

.PHONY: clean

tmpclean:
	rm *~
clean:
	rm $(OBJS) $(TARGET)
allclean:
	rm $(OBJS) $(TARGET)
	rm  -i *.txt *.raw