CC = gcc
C_FLAGS = -Wall -Wextra

all: savings_account_problem

savings_account_problem: sap.o
	$(CC) savings_account_problem.o -o sap

sap.o: savings_account_problem.c
	$(CC) -c $(C_FLAGS) savings_account_problem.c

clean:
	rm -f sap savings_account_problem.o
