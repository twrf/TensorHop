
###################################################################################################
#    author : strivehappy
#    date   : Jul 7 2018


###################################################################################################
#   target and architecture
###################################################################################################

EXEC    = tensorhop
ARCH    =
GPU     =
CUDNN   =
OPENCV  =
OPENMP  =
DEBUG   =


###################################################################################################
#   compile and link command
###################################################################################################

COMPILE =
CC      = $(COMPILE)gcc
CXX     = $(COMPILE)g++
LD      = $(COMPILE)ld
AR      = $(COMPILE)ar
ARFLAGS = -rc


###################################################################################################
#   source directory path
###################################################################################################

PRJ_DIR                         =   .

###################################################################################################
#   dependencies
###################################################################################################

OBJS = \
        $(PRJ_DIR)/src/TensorHop.o                      \
        $(PRJ_DIR)/src/jsoncpp/jsoncpp.o                \
        $(PRJ_DIR)/src/format/Bmp.o                     \
        $(PRJ_DIR)/src/format/Image.o                   \
        $(PRJ_DIR)/src/common/Defs.o                    \
        $(PRJ_DIR)/src/utils/PrintLog.o                 \
        $(PRJ_DIR)/src/utils/Mutex.o                    \
        $(PRJ_DIR)/src/utils/Tools.o                    \
        $(PRJ_DIR)/src/utils/math_function.o            \
        $(PRJ_DIR)/src/image_process/morph.o            \
        $(PRJ_DIR)/src/net/Net.o                        \
        $(PRJ_DIR)/src/layer/LayerBase.o                \
        $(PRJ_DIR)/src/layer/LayerConvolution.o         \
        $(PRJ_DIR)/src/layer/LayerInnerProduct.o        \
        $(PRJ_DIR)/src/layer/LayerInput.o               \
        $(PRJ_DIR)/src/layer/LayerPooling.o             \
        $(PRJ_DIR)/src/layer/LayerRelu.o                \
        $(PRJ_DIR)/src/layer/LayerSoftmaxWithLoss.o

CFLAGS  = -Wall -fPIC -g -std=c++11 -O3
CFLAGS  += -fopenmp
# -Wno-psabi -Wno-deprecated


INC_PATH_COMMON_LIB      =   

INC_PATH_THIRDPARTY_LIB  =      -I/usr/local/include/opencv -I/usr/local/include

INC_PATH_THIS_PROJECT    =      -I$(PRJ_DIR)/include/ -I$(PRJ_DIR)/src/

LIB_PATH_COMMON_LIB      =

LIB_COMMON_LIB           =

LIB_PATH_THIRDPARTY_LIB  =  -L/usr/local/lib

LIB_THIRDPARTY_LIB       =  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d \
                            -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml \
                            -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching \
                            -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

LIB_OTHERS               =  -pthread -lm -ldl

###################################################################################################
#   Primary link
###################################################################################################

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) -o $(EXEC) $(OBJS) $(CFLAGS) $(LIB_OTHERS) $(LIB_PATH_COMMON_LIB) $(LIB_COMMON_LIB)
	rm -f -R ./release
	mkdir ./release
	mv $(EXEC) ./release

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $(INC_PATH_COMMON_LIB) $(INC_PATH_THIS_PROJECT) $< -o $@


clean:
	rm -f  ./release/$(EXEC) ./release/$(OBJS) ./$(OBJS) ./$(EXEC) ./release/* ./*.bmp

