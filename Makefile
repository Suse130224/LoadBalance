all: log.o host.o healthCheck.o loadBalance.o loadBalancer

log.o: log.cpp log.h
	g++ -c log.cpp -o log.o
host.o: host.cpp host.h
	g++ -std=c++11 -c host.cpp -o host.o
loadBalance.o: loadBalance.cpp loadBalance.h
	g++ -std=c++11 -c loadBalance.cpp -o loadBalance.o
healthCheck.o: healthCheck.cpp healthCheck.h
	g++ -std=c++11 -c healthCheck.cpp -o healthCheck.o
util.o: util.cpp util.h
	g++ -std=c++11 -c util.cpp -o util.o
algorithmFactory.o: algorithmFactory.cpp algorithmFactory.h
	g++ -std=c++11 -c algorithmFactory.cpp -o algorithmFactory.o
loadBalancer: host.o loadBalance.o healthCheck.o main.cpp log.o util.o algorithmFactory.o
	g++ -std=c++11 host.o loadBalance.o healthCheck.o log.o util.o  algorithmFactory.o main.cpp -o loadBalancer -pthread

clean:
	\rm *.o loadBalancer
