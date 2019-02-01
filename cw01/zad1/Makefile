CC = gcc -Wall -w
LIB = lib
OUTNAME = zad3
MAIN = main


run_shared: shared
		./$(OUTNAME)_shared 128 256 1 1 15

run_dynamic: dynamic
		./$(OUTNAME)_dynamic

run_static: static
		./$(OUTNAME)_static




		
shared:

	# zbuduj biblioteke wspoldzielona .so
	$(CC) -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -Wl,-rpath=. -L. -o $(OUTNAME)_shared $(MAIN).c -l$(LIB)
		
		

static:
	# kompilacja zrodla
	$(CC) -c $(LIB).c -o $(LIB).o

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB).a $(LIB).o

	# linkowanie
	$(CC) -static $(MAIN).c -L. -l$(LIB) -o $(OUTNAME)_static	

sharedO1:

	# zbuduj biblioteke wspoldzielona .so
	$(CC) -O1 -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -Wl,-rpath=. -L. -o $(OUTNAME)_shared $(MAIN).c -l$(LIB)
		
		

staticO1:
	# kompilacja zrodla
	$(CC) -c $(LIB).c -o $(LIB).o

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB).a $(LIB).o

	# linkowanie
	$(CC) -O1 -static $(MAIN).c -L. -l$(LIB) -o $(OUTNAME)_static	




sharedO2:

	# zbuduj biblioteke wspoldzielona .so
	$(CC) -O2 -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -Wl,-rpath=. -L. -o $(OUTNAME)_shared $(MAIN).c -l$(LIB)
		
		

staticO2:
	# kompilacja zrodla
	$(CC) -c $(LIB).c -o $(LIB).o

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB).a $(LIB).o

	# linkowanie
	$(CC) -O2 -static $(MAIN).c -L. -l$(LIB) -o $(OUTNAME)_static	
	
sharedO3:

	# zbuduj biblioteke wspoldzielona .so
	$(CC) -O3 -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -Wl,-rpath=. -L. -o $(OUTNAME)_shared $(MAIN).c -l$(LIB)
		
		

staticO3:
	# kompilacja zrodla
	$(CC) -c $(LIB).c -o $(LIB).o

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB).a $(LIB).o

	# linkowanie
	$(CC) -O3 -static $(MAIN).c -L. -l$(LIB) -o $(OUTNAME)_static		
	

sharedOs:

	# zbuduj biblioteke wspoldzielona .so
	$(CC) -Os -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -Wl,-rpath=. -L. -o $(OUTNAME)_shared $(MAIN).c -l$(LIB)
		
		

staticOs:
	# kompilacja zrodla
	$(CC) -c $(LIB).c -o $(LIB).o

	# zbuduj biblioteke statyczna .a
	ar rcs lib$(LIB).a $(LIB).o

	# linkowanie
	$(CC) -Os -static $(MAIN).c -L. -l$(LIB) -o $(OUTNAME)_static	



dynamic:
	# zbuduj biblioteke wspoldzielona .so
	$(CC) -fPIC -c $(LIB).c -o $(LIB).o
	$(CC) -Wl,-rpath=. -fPIC -shared -o lib$(LIB).so $(LIB).o

	# linkowanie
	$(CC) -L. -o $(OUTNAME)_dynamic $(MAIN).c -ldl -D DLL

	# czyscimy
	rm -f*.0

clean:
	rm -f *.o
	
		
		


		

