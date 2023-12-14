all: out/test-size out/write-after-reader-close out/read-after-writer-close out/read-when-pipe-empty out/write-when-pipe-full

out/test-size: src/test-size.c
	gcc -o out/test-size src/test-size.c

out/write-after-reader-close: src/write-after-reader-close.c
	gcc -o out/write-after-reader-close src/write-after-reader-close.c

out/read-after-writer-close: src/read-after-writer-close.c
	gcc -o out/read-after-writer-close src/read-after-writer-close.c

out/read-when-pipe-empty: src/read-when-pipe-empty.c
	gcc -o out/read-when-pipe-empty src/read-when-pipe-empty.c

out/write-when-pipe-full: src/write-when-pipe-full.c
	gcc -o out/write-when-pipe-full src/write-when-pipe-full.c

clean:
	rm -f out/*
