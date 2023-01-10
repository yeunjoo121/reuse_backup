CC=gcc
OBJS=hils_pi.o ft_split.o linked_deque.o
TARGET=a.out

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -lm -lwiringPi -lwiringPiDev

hils_pi.o: hils_pi.h hils_pi.c
ft_split.o: ft_split.c
linked_deque.o: linked_deque.c

clean:
	rm -f *.o
	rm -f $(TARGET)
