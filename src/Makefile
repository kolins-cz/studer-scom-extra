CC := gcc
CFLAGS := -g
LIBS := -lmosquitto

OBJECTS := ../scomlib_extra/scomlib_extra.o ../scomlib_extra/scomlib_extra_errors.o ../scomlib/scom_data_link.o ../scomlib/scom_property.o serial.o main.o

.PHONY: all clean

all: studer232-to-mqtt

clean:
	rm -f $(OBJECTS) studer232-to-mqtt

studer232-to-mqtt: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o studer232-to-mqtt

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
