CC = gcc
CFLAGS = -lm -lpigpio -pthread -lrt
SRC = electraAcRemote.c electraAcRemoteEncoder.c
TARGET = electraAcRemote

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)
