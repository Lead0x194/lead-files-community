CC	= g++
TARGET = teen.exe
IFLAGS = -I./INCLUDE -I../../
CFLAGS	= $(IFLAGS) -Wall -O2 -pipe -g
LIBS	= -L./LIB -lmysqlclient -lz -lm

SRCS	= main.c utils.c socket.c signal.c input.c config.c \
	   libsql.c sql.c log.c tellwait.c sql_account.c event.c priority_queue.c

OBJS	= $(SRCS:%.c=%.o)


default: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -static -o ../$(TARGET) $(OBJS) $(LIBS)
	chmod 700 ../$(TARGET)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o ../$(TARGET)

dep:
	$(CC) -MM $(SRCS) > Depend

install:
	cp -f ../$(TARGET) $(HOME)/TEST/chat

sinclude Depend
