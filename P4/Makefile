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
	@for t in $(TEST_OUTPUTS); do \
		echo "Running $$t..."; \
		$$t; \
		echo ""; \
	done

# run scripts
TESTS: all
	@chmod +x tests/correctFormat.csh
	@chmod +x tests/multiplayer.csh
	@chmod +x tests/testOpen.csh	
	@chmod +x tests/testMove.csh	
	@chmod +x tests/testFail.csh	
	@chmod +x tests/testUnknown.csh	
	@chmod +x tests/sessionOne.csh	
	@chmod +x tests/sessionTwo.csh	
	@chmod +x tests/doubleOpen.csh	
	@chmod +x tests/moveBeforeName.csh	
	@chmod +x tests/moveNotTurn.csh	
	@chmod +x tests/notAllowedMovePileIndex.csh	
	@chmod +x tests/notAllowedMoveQuantity.csh	
	@./tests/correctFormat.csh
	@./tests/multiplayer.csh 
	@./tests/testOpen.csh
	@./tests/testMove.csh
	@./tests/testFail.csh
	@./tests/testUnknown.csh
	@./tests/sessionOne.csh
	@./tests/sessionTwo.csh
	@./tests/doubleOpen.csh
	@./tests/moveBeforeName.csh
	@./tests/moveNotTurn.csh
	@./tests/notAllowedMovePileIndex.csh
	@./tests/notAllowedMoveQuantity.csh

	
.PHONY: TESTS

# remove nimd.o and all built test outputs
clean:
	rm -f -rf $(BUILD_FOLDER)/* nimd rawc