Server : test.cc Socket.cc Protocol.cc
	g++ -std=c++20 test.cc Socket.cc Protocol.cc -o Server -ljsoncpp

.PHONY:clean
clean: 
	rm -f Server 
	echo "" > log.txt

.PHONY:concept_test
concept_test : concept_test.cc
	g++ -std=c++20 concept_test.cc -o concept_test


