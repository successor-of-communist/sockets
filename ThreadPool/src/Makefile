a.out: ser_mul_thread.o
	g++ -o a.out ser_mul_thread.o -lpthread

Error.o: Error.h
	g++ -c Error.h

rw.o: rw.h Error.h
	g++ -c rw.h Error.h

Socket.o: Socket.h Error.h
	g++ -c Socket.h Error.h

ThreadPool.o: ThreadPool.h header.h rw.h Socket.h
	g++ -c ThreadPool.h header.h rw.h Socket.h

ser_mul_thread.o: ser_mul_thread.cpp header.h Socket.h Error.h ThreadPool.h
	g++ -c ser_mul_thread.cpp

clean:
	rm *.o
