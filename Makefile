CC = gcc
C_FLAGS = -Wall -Wextra

program_NAME = sap
withdraw_NAME = withdraw
#receiver_NAME = receiver

helper_C_SRCS = Semaphore.c SharedMemory.c LinkedList.c
main_program_C_SRCS = Hw4.c
withdraw_C_SRCS = Withdraw.c
#receiver_C_SRCS = Receiver.c

helper_C_OBJS = ${helper_C_SRCS:.c=.o}
main_program_C_OBJS = ${main_program_C_SRCS:.c=.o}
withdraw_C_OBJS = ${withdraw_C_SRCS:.c=.o}
#receiver_C_OBJS = ${receiver_C_SRCS:.c=.o}

all: $(program_NAME) $(withdraw_NAME)

$(program_NAME): $(main_program_C_OBJS) $(helper_C_OBJS)
	$(CC) $(helper_C_OBJS) $(main_program_C_OBJS) $(C_FLAGS) -o $(program_NAME)

$(withdraw_NAME): $(withdraw_C_OBJS) $(helper_C_OBJS)
	$(CC) $(helper_C_OBJS) $(withdraw_C_OBJS) $(C_FLAGS) -o $(withdraw_NAME)

#$(receiver_NAME): $(receiver_C_OBJS) $(helper_C_OBJS)
#	$(CC) $(helper_C_OBJS) $(receiver_C_OBJS) $(C_FLAGS) -o $(receiver_NAME)

clean: 
	@- rm $(program_NAME) $(withdraw_NAME)
	@- rm $(helper_C_OBJS) $(main_program_C_OBJS) $(withdraw_C_OBJS)
