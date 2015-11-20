######################################################################
#  PCoroutine project Makefile
######################################################################

include Makefile.incl

.PHONY: all clean clobber

all:	libpcoroutine.a testcr

posix:	posix.o esp8266.o
	$(GXX) posix.o esp8266.o -o posix

libpcoroutine.a: pcoroutine.o
	@rm -f libcoroutine.a
	ar cr libpcoroutine.a pcoroutine.o

testcr:	libpcoroutine.a testcr.o
	$(GXX) testcr.o -o testcr libpcoroutine.a

clean:
	rm -f *.o

clobber: clean
	rm -f libpcoroutine.a testcr

pcoroutine.o: pcoroutine.hpp
testcr.o: pcoroutine.hpp

# End
