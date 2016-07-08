#Makefile template for CS370

# List of files
C_SRCS          = scheduler.c
C_OBJS          = scheduler.o
C_HEADERS       = scheduler.h

OBJS            = ${C_OBJS}
EXE             = Scheduler

# Compiler and loader commands and flags
GCC             = gcc
GCC_FLAGS       = -g -std=c99 -Wall -c -O0
LD_FLAGS        = -g -std=c99 -Wall -O0

defualt: all

# Compile .c files to .o files
%.o: %.c
	$(GCC) $(GCC_FLAGS) $<

# Target is the executable
all: $(OBJS)
	$(GCC) $(LD_FLAGS) $(OBJS) -o $(EXE)

# Recompile C objects if headers change
${C_OBJS}:      ${C_HEADERS}

# Clean up the directory
clean:
	rm -f *.o *~ $(EXE)

# make package
package:
	rm -f *.o *~ *.tar $(EXE)
	tar -cvf scheduler.tar *.c *.h README.txt Makefile
