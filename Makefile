CPP = g++
BIN = whitenoise
SRC = main.cpp
LIBS = SDL_image
CXXFLAGS = `pkg-config --cflags --libs $(LIBS)` -ljpeg -Wall

all: $(BIN)

$(BIN): $(SRC)
	$(CPP) -o $(BIN) $(SRC) $(CXXFLAGS)

clean:
	rm $(BIN)
	rm -r output
