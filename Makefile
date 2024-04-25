CC := gcc
CFLAGS := -g
LIBS := -lmosquitto

OBJECTS := scomlib_extra/scomlib_extra.o scomlib_extra/scomlib_extra_errors.o scomlib/scom_data_link.o scomlib/scom_property.o src/serial.o src/main.o

.PHONY: all clean

all: bin/studer232-to-mqtt

clean:
	rm -f $(OBJECTS) bin/studer232-to-mqtt

bin/studer232-to-mqtt: $(OBJECTS)
	mkdir -p ../bin
	$(CC) $(OBJECTS) $(LIBS) -o bin/studer232-to-mqtt

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<