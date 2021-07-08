run:
	xmake
	cp ./build/linux/x86_64/debug/libluarime.so ./test
	cd test && lua test.lua

