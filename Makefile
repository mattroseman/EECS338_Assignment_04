CC = gcc
C_FLAGS = -Wall -Wextra

program_NAME = sap
program_C_SRCS = $(wildcard *.c)
program_C_OBJS = ${program_C_SRCS:.c=.o}

all: $(program_NAME)

$(program_NAME): $(program_C_OBJS)
	$(CC)  $(program_C_OBJS) $(C_FLAGS) -o $(program_NAME)

clean: 
	@- rm $(program_NAME)
	@- rm $(program_C_OBJS)
