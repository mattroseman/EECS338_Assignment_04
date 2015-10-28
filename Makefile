CC = gcc
C_FLAGS = -Wall -Wextra

program_NAME = sap
withdraw_NAME = withdraw
deposit_NAME = deposit

helper_C_SRCS = Semaphore.c SharedMemory.c
main_program_C_SRCS = Hw4.c
withdraw_C_SRCS = Withdraw.c
deposit_C_SRCS = Deposit.c

helper_C_OBJS = ${helper_C_SRCS:.c=.o}
main_program_C_OBJS = ${main_program_C_SRCS:.c=.o}
withdraw_C_OBJS = ${withdraw_C_SRCS:.c=.o}
deposit_C_OBJS = ${deposit_C_SRCS:.c=.o}

all: $(program_NAME) $(withdraw_NAME) $(deposit_NAME)

$(program_NAME): $(main_program_C_OBJS) $(helper_C_OBJS)
	$(CC) $(helper_C_OBJS) $(main_program_C_OBJS) $(C_FLAGS) -o $(program_NAME)

$(withdraw_NAME): $(withdraw_C_OBJS) $(helper_C_OBJS)
	$(CC) $(helper_C_OBJS) $(withdraw_C_OBJS) $(C_FLAGS) -o $(withdraw_NAME)

$(deposit_NAME): $(deposit_C_OBJS) $(helper_C_OBJS)
	$(CC) $(helper_C_OBJS) $(deposit_C_OBJS) $(C_FLAGS) -o $(deposit_NAME)

clean: 
	@- rm $(program_NAME) $(withdraw_NAME) $(deposit_NAME)
	@- rm $(helper_C_OBJS) $(main_program_C_OBJS) $(withdraw_C_OBJS) $(deposit_C_OBJS)
