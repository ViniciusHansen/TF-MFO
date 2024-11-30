all:
	g++ -I lib test.cpp && ./a.out
	
clean:
	rm -f a.out

