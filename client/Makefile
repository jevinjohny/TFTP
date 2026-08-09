CC = gcc
CFLAGS = -Wall -Wextra

TARGET = tftp_client

CLIENT_SRC = tftp_client.c
COMMON_SRC = ../common/tftp.c

$(TARGET): $(CLIENT_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) $(COMMON_SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)