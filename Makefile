#Makfile:	  Makefile for all the code
#__author__      = "Nitin Mohan
#__copyright__   = "Copy Right 2018. NM Technologies"


SOURCE_DIR	= bin
OBJ_DIR		= Obj
INC_DIR		= inc
OUT_DIR		= /opt/PhotoNetwork/bin
CFLAGS 		= -g -Wall -I $(INC_DIR)

output: $(OBJ_DIR)/pn_controller.o $(OBJ_DIR)/srv_capture.o
		gcc $(OBJ_DIR)/pn_controller.o -ljson-c -o $(OUT_DIR)/photonetwork
		gcc $(OBJ_DIR)/srv_capture.o -o $(OUT_DIR)/srv_capture

$(OBJ_DIR)/pn_controller.o: $(SOURCE_DIR)/pn_controller.c $(INC_DIR)/pn_controller.h
							gcc $(CFLAGS) -c $(SOURCE_DIR)/pn_controller.c -o $@

$(OBJ_DIR)/srv_capture.o: $(SOURCE_DIR)/srv_capture.c
							gcc $(CFLAGS) -c $(SOURCE_DIR)/srv_capture.c -o $@
clean:
	rm $(OBJ_DIR)/*.o $(OUT_DIR)/photonetwork $(OUT_DIR)/srv_capture
