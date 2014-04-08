#-----------------------------------------------------------------------------
# Makefile for validation
#
#  This does output validation for the kmltest program on a number of test
#  inputs.  The inputs are of the form test*.in and the saved outputs are
#  in test*.save.  This assumes that the src/kmltest program has been
#  compiled with -O3 and the -DPERF option.  (Otherwise Performance data
#  will not be printed.)
#
#	make 				do all validation runs
#	make clean			delete temporary files
#	make realclean			(same as clean)
#	make update			copy the .tmp files to the .save
#					files
#
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Basic definitions
#	BIN_DIR			directory with executable
#	KMLTEST			path to the test program
#-----------------------------------------------------------------------------

BIN_DIR = ../bin
KMLTEST = $(BIN_DIR)/kmltest
KMLMINIMAL = $(BIN_DIR)/kmlminimal
KMLSAMPLE = $(BIN_DIR)/kmlsample

#-----------------------------------------------------------------------------
# Validation
#-----------------------------------------------------------------------------
.IGNORE:

validate: validateMinimal validateSample validate0 validate1 validate2 \
	validate3 validate4 validate5 validate6 validate7

validateMinimal:
	$(KMLMINIMAL) > testMin.tmp
	diff -w testMin.save testMin.tmp > testMin.diff
	more testMin.diff

validateSample:
	$(KMLSAMPLE) -d 2 -k 4 -max 20 -df test1-dat.txt -s 20 > testSamp.tmp
	diff -w testSamp.save testSamp.tmp > testSamp.diff
	more testSamp.diff

validate0:
	$(KMLTEST) < test0.in > test0.tmp
	diff -w test0.save test0.tmp > test0.diff
	more test0.diff

validate1:
	$(KMLTEST) < test1.in > test1.tmp
	diff -w test1.save test1.tmp > test1.diff
	more test1.diff

validate2:
	$(KMLTEST) < test2.in > test2.tmp
	diff -w test2.save test2.tmp > test2.diff
	more test2.diff

validate3:
	$(KMLTEST) < test3.in > test3.tmp
	diff -w test3.save test3.tmp > test3.diff
	more test3.diff

validate4:
	$(KMLTEST) < test4.in > test4.tmp
	diff -w test4.save test4.tmp > test4.diff
	more test4.diff

validate5:
	$(KMLTEST) < test5.in > test5.tmp
	diff -w test5.save test5.tmp > test5.diff
	more test5.diff

validate6:
	$(KMLTEST) < test6.in > test6.tmp
	diff -w test6.save test6.tmp > test6.diff
	more test6.diff

validate7:
	$(KMLTEST) < test7.in > test7.tmp
	diff -w test7.save test7.tmp > test7.diff
	more test7.diff

update:
	mv testMin.tmp testMin.save
	mv testSamp.tmp testSamp.save
	mv test0.tmp test0.save
	mv test1.tmp test1.save
	mv test2.tmp test2.save
	mv test3.tmp test3.save
	mv test4.tmp test4.save
	mv test5.tmp test5.save
	mv test6.tmp test6.save
	mv test7.tmp test7.save
	-rm -f *.diff

#-----------------------------------------------------------------------------
# Cleaning
#-----------------------------------------------------------------------------

clean:
	-rm -f *.tmp *.diff *.dmp core

realclean: clean
