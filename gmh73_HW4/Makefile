# I want to use the GCC compiler
COMPILER = gcc
# My program file is cars.c...
SOURCE = cars.c
# ...and I want my output to go to cars
OUTPUT = cars

# When make is run with no arguments, do this stuff
all:
	# Call GCC to do weird things with my program file
	$(COMPILER) -o $(OUTPUT) $(SOURCE)
	# Make it so that we can actually run our program
	chmod 700 $(OUTPUT)

# Clean up after ourselves
clean:
	# Note to self: don't do rm -rf /
	rm -f $(OUTPUT)


