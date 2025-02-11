CC=g++
TARGET_TAGE=demo_tage
TARGET_LTAGE=demo_ltage
TARGET_TAGE_IO=tage_IO
TARGET_BTB=demo_btb
TARGET_FRONTED=fronted_pred
TARGET_TEST_ENV=test

GDB_TARGET_TAGE=demo_tage.gdb
GDB_TARGET_LTAGE=demo_ltage.gdb
GDB_TARGET_TAGE_IO=tage_IO.gdb
GDB_TARGET_TEST_ENV=test.gdb

SRC_LTAGE = BPU/dir_predictor/demo_loop.cpp BPU/dir_predictor/utils.cpp BPU/dir_predictor/demo_ltage.cpp
SRC_FRONTED = fronted_main.cpp BPU/dir_predictor/demo_tage.cpp BPU/target_predictor/btb.cpp
SRC_TEST_ENV = test_env.cpp front_top.cpp BPU/BPU_top.cpp icache/icache.cpp fifo/instruction_FIFO.cpp fifo/PTAB.cpp BPU/dir_predictor/demo_tage.cpp BPU/target_predictor/btb.cpp

.PHONY: all tage tage_gdb clean build ltage ltage_gdb tageIO tageIO_gdb btb fronted test_env test_env_gdb

build: $(TARGET_TAGE) $(TARGET_LTAGE) $(TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV)

all: $(TARGET_TAGE) $(GDB_TARGET_TAGE) $(TARGET_LTAGE) $(GDB_TARGET_LTAGE) $(TARGET_TAGE_IO) $(GDB_TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV) $(GDB_TARGET_TEST_ENV)

$(TARGET_TAGE): BPU/dir_predictor/demo_tage.cpp
	$(CC) -O3 -w -o $@ $<

$(GDB_TARGET_TAGE): BPU/dir_predictor/demo_tage.cpp
	$(CC) -g -w -o $@ $<

$(TARGET_LTAGE): $(SRC_LTAGE)
	$(CC) -O3 -w -o $@ $(SRC_LTAGE)

$(GDB_TARGET_LTAGE): $(SRC_LTAGE)
	$(CC) -g -w -o $@ $(SRC_LTAGE)
	
$(TARGET_TAGE_IO): BPU/dir_predictor/tage_IO.cpp
	$(CC) -O3 -w -o $@ $<

$(GDB_TARGET_TAGE_IO): BPU/dir_predictor/tage_IO.cpp
	$(CC) -g -w -o $@ $<

$(TARGET_BTB): BPU/target_predictor/btb.cpp
	$(CC) -O3 -w -o $@ $<

$(TARGET_FRONTED): $(SRC_FRONTED)
	$(CC) -O3 -w -o $@ $(SRC_FRONTED)

$(TARGET_TEST_ENV): $(SRC_TEST_ENV)
	$(CC) -O3 -w -o $@ $(SRC_TEST_ENV)

$(GDB_TARGET_TEST_ENV): $(SRC_TEST_ENV)
	$(CC) -g -w -o $@ $(SRC_TEST_ENV)

tage: $(TARGET_TAGE)
	./$(TARGET_TAGE) > tage_log2

btb: $(TARGET_BTB)
	./$(TARGET_BTB) > btb_log

fronted: $(TARGET_FRONTED)
	./$(TARGET_FRONTED) > fronted_log

tage_gdb: $(GDB_TARGET_TAGE)
	gdb --args ./$(GDB_TARGET_TAGE)
	
ltage: $(TARGET_LTAGE)
	./$(TARGET_LTAGE) > ltage_log

ltage_gdb: $(GDB_TARGET_LTAGE)
	gdb --args ./$(GDB_TARGET_LTAGE)

tageIO: $(TARGET_TAGE_IO)
	./$(TARGET_TAGE_IO) > tage_IO_log
	./$(TARGET_TAGE) >> tage_IO_log

tageIO_gdb: $(GDB_TARGET_TAGE_IO)
	gdb --args ./$(GDB_TARGET_TAGE_IO)

testenv: $(TARGET_TEST_ENV)
	./$(TARGET_TEST_ENV) > test_env_log

testenv_gdb: $(GDB_TARGET_TEST_ENV)
	gdb --args ./$(GDB_TARGET_TEST_ENV)

clean:
	rm -f $(TARGET_TAGE) $(GDB_TARGET_TAGE) $(TARGET_LTAGE) $(GDB_TARGET_LTAGE) $(TARGET_TAGE_IO) $(GDB_TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV) $(GDB_TARGET_TEST_ENV)
	rm -f ltage_log tage_log loop_log tage_IO_log btb_log fronted_log test_env_log

