# Makefile Written by ticktick

# Show how to cross-compile c/c++ code for android platform

.PHONY: clean
#-------------------------------------------------------
CUR_DIR = ../../../A-src-linux/schedule
#-------------------------------------------------------
TARGET = bin/schedule
#-------------------------------------------------------
#-------------------------------------------------------
#-------------------------------------------------------
#-------------------------------------------------------
CC = gcc
CXX = g++
AR = ar
LD = ld
STRIP = strip
WALL = -Wall
#-------------------------------------------------------
INCLUDE = -I$(CUR_DIR)/src \
			-I$(CUR_DIR)/../../A-all-common/base/inc \
			-I$(CUR_DIR)/../../A-all-common/user/inc \
			-I$(CUR_DIR)/../common/inc \
			-I/usr/local/include \
			-I/usr/local/event2 \
			-I/usr/local/include/json-c \
			-I/usr/local/include/openssl \
#-------------------------------------------------------
STATIC_LIB = /usr/local/lib/libevent.a \
				/usr/local/lib/libevent_openssl.a \
				/usr/local/lib/libjson-c.a \
				/usr/local/lib/libssl.a \
				/usr/local/lib/libcrypto.a \
#-------------------------------------------------------
SHARE_LIB_PATH = 
SHARE_LIB = -ldl -lz -lpthread
#-------------------------------------------------------
OBJ_DIR = obj
#-------------------------------------------------------
#获取当前目录下所有的.c文件
SOURCE = $(wildcard $(CUR_DIR)/src/*.c) \
		$(wildcard $(CUR_DIR)/../../A-all-common/base/src/*.c) \
		$(wildcard $(CUR_DIR)/../../A-all-common/user/src/*.c) \
		$(wildcard $(CUR_DIR)/../common/src/*.c)
#-------------------------------------------------------
#得到相应的.o文件,所有的.c文件替换为.o文件
OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SOURCE)))
#-------------------------------------------------------
$(OBJ_DIR)/%.o:$(CUR_DIR)/../../A-all-common/base/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/../../A-all-common/user/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/../common/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(INCLUDE) $< -c -o $@
#-------------------------------------------------------
#生成目标的依赖
$(TARGET): $(OBJ)
	@echo "--------------create $(TARGET)---------"
	@$(CC) $(WALL) $(SHARE_LIB_PATH) -o $@ $^ $(STATIC_LIB) $(SHARE_LIB)
	$(STRIP) $(TARGET)
	ls -alh $(TARGET)
#-------------------------------------------------------
clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(TARGET)
#-------------------------------------------------------