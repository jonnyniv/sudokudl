CC=g++

SRCS=node.cpp main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

sudokudlx: $(OBJS)
	$(CC) -o sudokudlx $(OBJS)

%.o: %.cpp
	$(CC) -c $<

clean:
	rm $(OBJS)