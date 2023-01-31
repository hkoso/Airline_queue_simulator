.phony all:
all: ACS

ACS: acs.c linked_list.c
	gcc -Wall acs.c linked_list.c -pthread -o ACS

.PHONY clean:
clean:
	-rm -rf *.o *.exe
