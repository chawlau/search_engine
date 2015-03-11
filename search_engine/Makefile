SRC_DIR := ./src
INC_DIR := ./include
EXE_DIR := ./bin
EXE := $(EXE_DIR)/main
SRC_OBJS := $(wildcard  $(SRC_DIR)/*.cpp )
INC_OBJS := $(wildcard $(INC_DIR)/*.h)
.PHONY:clean build 
clean :
	rm -f $(EXE) 
$(EXE):$(SRC_OBJS) $(INC_OBJS)
	g++ -g -o $@ $(SRC_OBJS) -lpthread -llog4cpp -ljson -Wno-deprecated -I$(INC_DIR) 

build:clean  $(EXE)

