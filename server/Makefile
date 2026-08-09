CC = gcc
CFLAGS = -Wall -Wextra

TARGET = tftp_server

SERVER_SRC = tftp_server.c
COMMON_SRC = ../common/tftp.c

$(TARGET): $(SERVER_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) $(COMMON_SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)