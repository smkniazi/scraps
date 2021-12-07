build: 
	rustc main.rs -l ndbapiwrapper  -L ../lib/build/
#go build -x .	

run:
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./../lib/build:/usr/local/mysql/lib; ./main	
clean:
	rm main

