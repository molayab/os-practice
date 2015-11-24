
all: compile

compile:
	g++ -o evaluador src/*.cpp -lpthread -lrt -ggdb
	@mv evaluador bin/

clean:
	@rm -rf bin/*
