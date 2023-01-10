CC=g++
OBJS=hils_pi.o ft_split.o linked_deque.o utils.o socket_api.o
TARGET=a.out

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -lm -lwiringPi -lwiringPiDev

hils_pi.o: hils_pi.hpp hils_pi.cpp
ft_split.o: ft_split.cpp
linked_deque.o: linked_deque.cpp
utils.o: utils.cpp
socket_api.o: socket_api.cpp

clean:
	rm -f *.o
	rm -f $(TARGET)
