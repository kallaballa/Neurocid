#-----------------------------------------------------------------------------
# Makefile for the sample program
#
# To compile sample program, go to ../src and enter:
#	make sample
#
# Once compliled, to run it use:
#	make run			to execute the sample program
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Basic definitions
#		BIN_DIR		where to store executables
#		KMLMINIMAL	where the minimal sample executable resides
#		KMLSAMPLE	where the sample executable resides
#-----------------------------------------------------------------------------

BIN_DIR	  = ../bin
KMLMINIMAL = $(BIN_DIR)/kmlminimal
KMLSAMPLE = $(BIN_DIR)/kmlsample

#-----------------------------------------------------------------------------
# Run the sample programs
#-----------------------------------------------------------------------------

default: 
	@echo "Please specify desired sample program, 'sample' or 'minimal'"

minimal:
	$(KMLMINIMAL)

sample:
	$(KMLSAMPLE) -df data_pts.txt

#-----------------------------------------------------------------------------
# Cleaning
#-----------------------------------------------------------------------------

clean:
	-rm -f core

realclean: clean
	-rm -f *.out
