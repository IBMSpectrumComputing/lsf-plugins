# -----------------------------------------------------------------
# You should not have to modify this file unless you have LSF
# Installed in non-standard locations.
# -----------------------------------------------------------------

CC = gcc 
LSF_TOPDIR = ${LSF_SERVERDIR}/../..
INCLUDE_DIR = ${LSF_TOPDIR}/include
LIBS = -lnsl ${LSF_LIBDIR}/libbat.a ${LSF_LIBDIR}/liblsf.a 
#CFLAGS = -g -DDEBUG
CFLAGS = -fPIC
LNKFLAGS = -x -shared
MNAME := $(shell uname -m)

SOURCE_FILES = pam_lsf.c
OBJECT_FILES = pam_lsf.o 

pam_lsf.so: $(OBJECT_FILES)
	gcc -shared -o pam_lsf.so $(OBJECT_FILES) $(LIBS)

pam_lsf.o: pam_lsf.c 
	$(CC) -c $(CFLAGS) pam_lsf.c -I$(INCLUDE_DIR) 

clean:
	rm -f pam_lsf.so pam_lsf.o core*

install:
	if [ -d /lib64/security ]; then cp -p pam_lsf.so /lib64/security; fi;
	if [ -d /lib/$(MNAME)-linux-gnu/security ]; then cp -p pam_lsf.so /lib/$(MNAME)-linux-gnu/security; fi;

