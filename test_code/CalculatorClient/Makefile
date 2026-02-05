Server : test.cc Socket.cc Protocol.cc
	g++ -std=c++20 test.cc Socket.cc Protocol.cc -o Client -ljsoncpp

.PHONY:clean
clean: 
	rm -f Client 
	echo "" > log.txt

.PHONY:concept_test
concept_test : concept_test.cc
	g++ -std=c++20 concept_test.cc -o concept_test


