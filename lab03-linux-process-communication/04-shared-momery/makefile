all: sender receiver

sender: src/sender.c
	gcc -o out/sender src/sender.c

receiver: src/receiver.c
	gcc -o out/receiver src/receiver.c

clean:
	rm -rf out/*
