CC=gcc
OBJS=class.o ft_split.o linked_deque.o
TARGET=a.out

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -lm -lwiringPi -lwiringPiDev

class.o: ebimu_c.h class.c
ft_split.o: ft_split.c
linked_deque.o: linked_deque.c

clean:
	rm -f *.o
	rm -f $(TARGET)
