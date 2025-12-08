CC=gcc # compiler
CFLAGS= -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -fsanitize=address,undefined -I. -Isrc # compiler options

BUILD_FOLDER = builds

NIMD = nimd.c
NETWORK = clients/src/network.h # remove later
PBUF = clients/src/pbuf.h # remove later
CLIENTS = clients/src/

# gets all tests 
TESTS := $(wildcard tests/*.c)

TEST_OUTPUTS := $(patsubst tests/%.c, $(BUILD_FOLDER)/%, $(TESTS))

all: nimd rawc ${TEST_OUTPUTS} 

nimd: nimd.c $(NETWORK) $(PBUF) nimd.h clients/src/network.c helper.c
	@mkdir -p $(BUILD_FOLDER)
	$(CC) $(CFLAGS) nimd.c clients/src/network.c helper.c -o nimd

# remove rawc later
rawc: ${CLIENTS}rawc.o ${CLIENTS}pbuf.o ${CLIENTS}network.o
	$(CC) $(CFLAGS) -o $@ $^

# recipe to build each test output
$(BUILD_FOLDER)/%: tests/%.c
	@mkdir -p $(BUILD_FOLDER)
	$(CC) $(CFLAGS) -o $@ $^ helper.c ${CLIENTS}pbuf.o ${CLIENTS}network.o


# run a single test: make runTest TEST=someTest
runTest: all
	@echo "\nRunning $(TEST) ... \n"
	@./$(BUILD_FOLDER)/$(TEST)

# run all tests
runAllTests: all
	@./$(BUILD_FOLDER)/runTests

# remove nimd.o and all built test outputs
clean:
	rm -f -rf $(BUILD_FOLDER)/* nimd rawc