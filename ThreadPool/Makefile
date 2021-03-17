server: ser_mul_thread.o Socket.o ThreadPool.o
	g++ -o server ser_mul_thread.o Socket.o ThreadPool.o -lpthread

ThreadPool.o: ThreadPool.cpp header.h ThreadPool.h Socket.h
	g++ -c ThreadPool.cpp

Socket.o: Socket.cpp Socket.h header.h
	g++ -c Socket.cpp

ser_mul_thread.o: ser_mul_thread.cpp header.h Socket.h ThreadPool.h
	g++ -c ser_mul_thread.cpp

clean:
	rm *.o
