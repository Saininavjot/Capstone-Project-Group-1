
CC = gcc
LIB = -liobb -lpthread -lrt

CFLAGS = -c -Wall

FILE_FLAGS = $(CC) $(LIB) $(CFLAGS)

all: train_project

train_project: main.o push_button.o gpio_control.o buzzer.o distance_sensor.o uart_control.o
	$(CC) $(LIB) distance_sensor.o push_button.o gpio_control.o buzzer.o uart_control.o main.o -o train_project.o

main.o:	main.c
	$(FILE_FLAGS) main.c

push_button.o: push_button.c
	$(FILE_FLAGS) push_button.c

gpio_control.o: gpio_control.c
	$(FILE_FLAGS) gpio_control.c

buzzer.o: buzzer.c
	$(FILE_FLAGS) buzzer.c

distance_sensor.o: distance_sensor.c
	$(FILE_FLAGS) distance_sensor.c

uart_control.o: uart_control.c
	$(FILE_FLAGS) uart_control.c

clean:
	rm -rf *o main
