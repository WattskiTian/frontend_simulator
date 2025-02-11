CC=g++
TARGET_TAGE=demo_tage
TARGET_LTAGE=demo_ltage
TARGET_TAGE_IO=tage_IO
TARGET_BTB=demo_btb
TARGET_FRONTED=fronted_pred
TARGET_TEST_ENV=test

HOME_DIR = /home/watts/Enligtenments-simu/playground/front-end

GDB_TARGET_TAGE=demo_tage.gdb
GDB_TARGET_LTAGE=demo_ltage.gdb
GDB_TARGET_TAGE_IO=tage_IO.gdb
GDB_TARGET_TEST_ENV=test.gdb

SRC_DIR_PRED = BPU/dir_predictor
SRC_TARGET_PRED = BPU/target_predictor
SRC_ICACHE = icache
SRC_FIFO = fifo

SRC_LTAGE = $(wildcard $(SRC_DIR_PRED)/*.cpp)
SRC_FRONTED = fronted_main.cpp $(SRC_DIR_PRED)/demo_tage.cpp $(SRC_TARGET_PRED)/btb.cpp
SRC_TEST_ENV = test_env.cpp front_top.cpp BPU/BPU_top.cpp \
               $(wildcard $(SRC_ICACHE)/*.cpp) \
               $(wildcard $(SRC_FIFO)/*.cpp) \
               $(SRC_DIR_PRED)/demo_tage.cpp \
               $(SRC_TARGET_PRED)/btb.cpp

.PHONY: all tage tage_gdb clean build ltage ltage_gdb tageIO tageIO_gdb btb fronted test_env test_env_gdb

build: $(TARGET_TAGE) $(TARGET_LTAGE) $(TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV)

all: $(TARGET_TAGE) $(GDB_TARGET_TAGE) $(TARGET_LTAGE) $(GDB_TARGET_LTAGE) $(TARGET_TAGE_IO) $(GDB_TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV) $(GDB_TARGET_TEST_ENV)

$(TARGET_TAGE): $(SRC_DIR_PRED)/demo_tage.cpp
	$(CC) -O3 -w -o $@ $<

$(GDB_TARGET_TAGE): $(SRC_DIR_PRED)/demo_tage.cpp
	$(CC) -g -w -o $@ $<

$(TARGET_LTAGE): $(SRC_LTAGE)
	$(CC) -O3 -w -o $@ $(SRC_LTAGE)

$(GDB_TARGET_LTAGE): $(SRC_LTAGE)
	$(CC) -g -w -o $@ $(SRC_LTAGE)

$(TARGET_TAGE_IO): BPU/dir_predictor/tage_IO.cpp
	$(CC) -O3 -w -o $@ $<

$(GDB_TARGET_TAGE_IO): BPU/dir_predictor/tage_IO.cpp
	$(CC) -g -w -o $@ $<

$(TARGET_BTB): $(SRC_TARGET_PRED)/btb.cpp
	$(CC) -O3 -w -o $@ $<

$(TARGET_FRONTED): $(SRC_FRONTED)
	$(CC) -O3 -w -o $@ $(SRC_FRONTED)

$(TARGET_TEST_ENV): $(SRC_TEST_ENV)
	$(CC) -O3 -w -o $@ $(SRC_TEST_ENV)

$(GDB_TARGET_TEST_ENV): $(SRC_TEST_ENV)
	$(CC) -g -w -o $@ $(SRC_TEST_ENV)

tage: $(TARGET_TAGE)
	./$(TARGET_TAGE) > $(HOME_DIR)/log/tage_log

btb: $(TARGET_BTB)
	./$(TARGET_BTB) > $(HOME_DIR)/log/btb_log

fronted: $(TARGET_FRONTED)
	./$(TARGET_FRONTED) > $(HOME_DIR)/log/fronted_log

tage_gdb: $(GDB_TARGET_TAGE)
	gdb --args ./$(GDB_TARGET_TAGE)
	
ltage: $(TARGET_LTAGE)
	./$(TARGET_LTAGE) > $(HOME_DIR)/log/ltage_log

ltage_gdb: $(GDB_TARGET_LTAGE)
	gdb --args ./$(GDB_TARGET_LTAGE)

tageIO: $(TARGET_TAGE_IO)
	./$(TARGET_TAGE_IO) > $(HOME_DIR)/log/tage_IO_log
	./$(TARGET_TAGE) >> $(HOME_DIR)/log/tage_IO_log

tageIO_gdb: $(GDB_TARGET_TAGE_IO)
	gdb --args ./$(GDB_TARGET_TAGE_IO)

testenv: $(TARGET_TEST_ENV)
	./$(TARGET_TEST_ENV) > $(HOME_DIR)/log/test_env_log

testenv_gdb: $(GDB_TARGET_TEST_ENV)
	gdb --args ./$(GDB_TARGET_TEST_ENV)

clean:
	rm -f $(TARGET_TAGE) $(GDB_TARGET_TAGE) $(TARGET_LTAGE) $(GDB_TARGET_LTAGE) $(TARGET_TAGE_IO) $(GDB_TARGET_TAGE_IO) $(TARGET_BTB) $(TARGET_FRONTED) $(TARGET_TEST_ENV) $(GDB_TARGET_TEST_ENV)
	rm -f $(HOME_DIR)/log/ltage_log $(HOME_DIR)/log/tage_log $(HOME_DIR)/log/loop_log $(HOME_DIR)/log/tage_IO_log $(HOME_DIR)/log/btb_log $(HOME_DIR)/log/fronted_log $(HOME_DIR)/log/test_env_log

