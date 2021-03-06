# Makefile Written by ticktick

# Show how to cross-compile c/c++ code for android platform

.PHONY: clean
#-------------------------------------------------------
CUR_DIR = ../../../A-src-android/task
#-------------------------------------------------------
TARGET = bin/task
#-------------------------------------------------------
ANDROID_API = 12
ARCH_DIR = arch-arm
TOOLCHAINS = toolchains/arm-linux-androideabi-4.9
HOST = arm-linux-androideabi-
#-------------------------------------------------------
NDKROOT = /home/caixiwen/android-ndk-r11
PLATFORM = $(NDKROOT)/platforms/android-$(ANDROID_API)/$(ARCH_DIR)
#-------------------------------------------------------
CROSS_COMPILE = $(NDKROOT)/$(TOOLCHAINS)/prebuilt/linux-x86_64/bin/$(HOST)
#-------------------------------------------------------
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
WALL = -Wall
CFLAGS = -D__ANDROID_API__=$(ANDROID_API) -O2 -fPIC -DANDROID -DHAVE_PTHREAD -mfpu=neon -mfloat-abi=softfp -fuse-ld=bfd -pie -fPIE -D_STLP_USE_NEWALLOC -lm \
		--sysroot=$(PLATFORM) \
		-D__ANDROID_PLATFORM__ \
#-------------------------------------------------------
INCLUDE = -I$(PLATFORM)/usr/include \
			-I$(CUR_DIR)/src \
			-I$(CUR_DIR)/../../A-all-common/base/inc \
			-I$(CUR_DIR)/../../A-all-common/user/inc \
			-I$(CUR_DIR)/../common/inc \
			-I$(CUR_DIR)/../third/libevent/inc \
			-I$(CUR_DIR)/../third/libevent/inc/event2 \
			-I$(CUR_DIR)/../third/json-c/inc \
			-I$(CUR_DIR)/../third/zlib/inc \
			-I$(CUR_DIR)/../third/openssl/include \
			-I$(CUR_DIR)/../third/openssl/include/openssl \

#-------------------------------------------------------
STATIC_LIB = $(CUR_DIR)/../third/libevent/libevent.a \
				$(CUR_DIR)/../third/libevent/libevent_openssl.a \
				$(CUR_DIR)/../third/json-c/libjson-c.a \
				$(CUR_DIR)/../third/zlib/libz.a \
				$(CUR_DIR)/../third/openssl/armeabi-v7a/lib/libssl.a \
				$(CUR_DIR)/../third/openssl/armeabi-v7a/lib/libcrypto.a \

#-------------------------------------------------------
SHARE_LIB_PATH = -L$(NDKROOT)/sources/cxx-stl/stlport/libs/armeabi/ -L$(PLATFORM)/usr/lib
SHARE_LIB = 
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
	@$(CC) -ggdb $(WALL) $(CFLAGS) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/../../A-all-common/user/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(CFLAGS) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/../common/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(CFLAGS) $(INCLUDE) $< -c -o $@

$(OBJ_DIR)/%.o:$(CUR_DIR)/src/%.c
	@echo "CC $<"
	@$(CC) -ggdb $(WALL) $(CFLAGS) $(INCLUDE) $< -c -o $@
#-------------------------------------------------------
#生成目标的依赖
$(TARGET): $(OBJ)
	@echo "--------------create $(TARGET)---------"
	@$(CC) $(WALL) $(CFLAGS) $(SHARE_LIB_PATH) -o $@ $^ $(SHARE_LIB) $(STATIC_LIB)
	$(STRIP) $(TARGET)
	ls -alh $(TARGET)
#-------------------------------------------------------
clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(TARGET)
#-------------------------------------------------------
