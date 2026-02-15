
.PHONY:debug
Debug_exe : Socket.cc test.cc HttpMessageCodec.cc
	g++ -std=c++20 -DDEBUG=1 Socket.cc test.cc HttpMessageCodec.cc -o Debug_exe

.PHONY:clean 
clean: 
	rm -f Debug_exe
	echo "" > log.txt




