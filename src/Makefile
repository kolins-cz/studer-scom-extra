CC := gcc
CFLAGS := -g
LIBS := -lmosquitto

OBJECTS := ../scomlib_extra/scomlib_extra.o ../scomlib_extra/scomlib_extra_errors.o ../scomlib/scom_data_link.o ../scomlib/scom_property.o serial.o main.o

.PHONY: all clean

all: scomtest

clean:
	rm -f $(OBJECTS) scomtest

scomtest: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o scomtest

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
