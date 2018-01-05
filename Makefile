BIN	 = image/bin/main
1CC	 = g++
CC	 = /home/share/kf3/opt/arm_toolchain-x1/external-toolchain/bin/arm-none-linux-gnueabi-gcc
CPP	 = /home/share/kf3/opt/arm_toolchain-x1/external-toolchain/bin/arm-none-linux-gnueabi-g++
STRIP	 = /home/share/kf3/opt/arm_toolchain-x1/external-toolchain/bin/arm-none-linux-gnueabi-strip
INC	 = -I . -I ./include -I ./main  -I ./gui -I ./logic -I ./storage -I ./driver \
		-I ./lib/dpbaseLinux -I ./lib/minigui/include

LIB	 = -L ./lib/dpbaseLinux/lib -L ./lib/minigui/lib \
		-Xlinker "-(" -lpthread -lrt  -lasound \
		-lAudioCore -lAudioOut -lMP3dec -lfreetype -ljpeg \
		-lnetcfg -lSilkCodec -lOnVif -lwpa_cli -lDPCall -lDPSip -lz  -lpng -lminigui_ths -Xlinker "-)"


DEFINE = -D DPLINUX
#DEFINE = -D DPLINUX -D _DEBUG

USERFLAGS :=
DEBUGFLAGS :=
OPTIMIZEFLAGS =
CFLAGS	 = -c -Wall -O2 -rdynamic $(DEFINE)
CPPFLAGS = -c -Wall -O2 -rdynamic $(DEFINE)

define all-files-under
$(shell find $(1) -name "*."$(2) -and -not -name ".*" )
endef

define all-subdir-files  
$(call all-files-under, $(1),"c")
endef

TMP_CSRCS = $(call all-subdir-files, ".")
#TMP_CSRCS += $(call video-subdir-cpp-files)
TMP_CPPSRCS = $(wildcard *.cpp)

CPPSRCS = $(TMP_CPPSRCS)
CSRCS 	 = $(TMP_CSRCS)

#FILTER_OUT += $(call all-subdir-files, "./gui/m4_gui")
#CSRCS = $(filter-out $(FILTER_OUT), $(TMP_CSRCS))

COBJS	:= $(CSRCS:.c=.o)
CPPOBJS	:= $(CPPSRCS:.cpp=.o)

CFLAGS += $(DEBUGFLAGS)
CPPFLAGS += $(DEBUGFLAGS)
CFLAGS += $(OPTIMIZEFLAGS)
CPPFLAGS += $(OPTIMIZEFLAGS)
CFLAGS += $(USERFLAGS)
CPPFLAGS += $(USERFLAGS)

RES_PATH = image/res/
ETC_PATH = image/etc/
# $(COBJS) : %.o : %.c 表示所有.o文件依赖于.c 目标文件为COBJS依赖于.o
# $< 第一个依赖文件 $@ 目标文件 
all : $(BIN)

$(COBJS) : %.o : %.c
	@$(CC) -c $< -o $@ $(INC) $(CFLAGS) $(LIB)
$(CPPOBJS) : %.o : %.cpp
	@$(CPP) -c $< -o $@ $(INC) $(CPPFLAGS) $(LIB)

$(BIN) : $(COBJS) $(CPPOBJS)
	@$(CPP) -o $(BIN) $(COBJS) $(CPPOBJS) $(LIB)
	@-rm $(COBJS) $(CPPOBJS)
	@$(STRIP) $(BIN)
	@echo "build success!"
			
clean:
	@rm -f $(BIN) $(COBJS) $(CPPOBJS)
	@rm -rf $(RES_PATH) $(ETC_PATH)
	@echo "clean success!"
