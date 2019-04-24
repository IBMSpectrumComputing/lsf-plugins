# -----------------------------------------------------------------
# You should not have to modify this file unless you have LSF
# Installed in non-standard locations.
# -----------------------------------------------------------------

CC = gcc
LSF_TOPDIR = ${LSF_SERVERDIR}/../..
INCLUDE_DIR = ${LSF_TOPDIR}/include
LIBS = -lnsl -ldl -lm ${LSF_LIBDIR}/libbat.a ${LSF_LIBDIR}/liblsf.a 
CFLAGS = -fPIC
#CFLAGS = -fPIC
LNKFLAGS = -x -shared

SOURCE_FILES = elim.reaper.c
OBJECT_FILES = elim.reaper.o 

elim.reaper: $(OBJECT_FILES)
	gcc -o elim.reaper $(OBJECT_FILES) $(LIBS)

elim.reaper.o: elim.reaper.c 
	$(CC) -c $(CFLAGS) elim.reaper.c -I$(INCLUDE_DIR) 

clean:
	rm -f elim.reaper elim.reaper.o core*

install:
	cp -p elim.reaper ${LSF_SERVERDIR}

