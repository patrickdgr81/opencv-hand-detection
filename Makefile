CC = g++
OBJS := hand.o
TARGET := hand
CFLAGS := -Wall
LDFLAGS := -lm -lopencv_gpu -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET) video.avi

