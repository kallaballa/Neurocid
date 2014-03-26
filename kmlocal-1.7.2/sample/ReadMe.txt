The sources for the sample programs are stored in the src directory:

    src/kmlminimal.cpp	Minimal sample program
    src/kmlsample.cpp	A more interesting sample program

The executables are compiled by running "make" in the base directory or
"make all" in the src directory.  Both programs print results to the standard
output.  The minimal sample can be executed without any inputs:

    ../bin/kmlminimal
      -or-
    make minimal

The full sample can be run given the test input file:

    ../bin/kmlsample -df data_pts.txt
      -or-
    make sample

To see the complete set of options for the full sample, enter it without
any arguments:

    ../bin/kmlsample
