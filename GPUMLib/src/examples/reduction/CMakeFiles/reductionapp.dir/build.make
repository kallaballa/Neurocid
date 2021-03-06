# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/elchaschab/devel/TankWar/GPUMLib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/elchaschab/devel/TankWar/GPUMLib

# Include any dependencies generated for this target.
include src/examples/reduction/CMakeFiles/reductionapp.dir/depend.make

# Include the progress variables for this target.
include src/examples/reduction/CMakeFiles/reductionapp.dir/progress.make

# Include the compile flags for this target's objects.
include src/examples/reduction/CMakeFiles/reductionapp.dir/flags.make

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o: src/examples/reduction/CMakeFiles/reductionapp.dir/flags.make
src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o: src/examples/reduction/reductionapp.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/elchaschab/devel/TankWar/GPUMLib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o"
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && /usr/bin/g++-4.6   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/reductionapp.dir/reductionapp.cpp.o -c /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction/reductionapp.cpp

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/reductionapp.dir/reductionapp.cpp.i"
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && /usr/bin/g++-4.6  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction/reductionapp.cpp > CMakeFiles/reductionapp.dir/reductionapp.cpp.i

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/reductionapp.dir/reductionapp.cpp.s"
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && /usr/bin/g++-4.6  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction/reductionapp.cpp -o CMakeFiles/reductionapp.dir/reductionapp.cpp.s

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.requires:
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.requires

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.provides: src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.requires
	$(MAKE) -f src/examples/reduction/CMakeFiles/reductionapp.dir/build.make src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.provides.build
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.provides

src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.provides.build: src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o

# Object files for target reductionapp
reductionapp_OBJECTS = \
"CMakeFiles/reductionapp.dir/reductionapp.cpp.o"

# External object files for target reductionapp
reductionapp_EXTERNAL_OBJECTS =

src/examples/reduction/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o
src/examples/reduction/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/build.make
src/examples/reduction/reductionapp: /usr/local/cuda/lib64/libcudart.so
src/examples/reduction/reductionapp: src/libGPUMLibUtils.a
src/examples/reduction/reductionapp: /usr/local/cuda/lib64/libcudart.so
src/examples/reduction/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable reductionapp"
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/reductionapp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/examples/reduction/CMakeFiles/reductionapp.dir/build: src/examples/reduction/reductionapp
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/build

# Object files for target reductionapp
reductionapp_OBJECTS = \
"CMakeFiles/reductionapp.dir/reductionapp.cpp.o"

# External object files for target reductionapp
reductionapp_EXTERNAL_OBJECTS =

src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o
src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/build.make
src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: /usr/local/cuda/lib64/libcudart.so
src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: src/libGPUMLibUtils.a
src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: /usr/local/cuda/lib64/libcudart.so
src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp: src/examples/reduction/CMakeFiles/reductionapp.dir/relink.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable CMakeFiles/CMakeRelink.dir/reductionapp"
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/reductionapp.dir/relink.txt --verbose=$(VERBOSE)

# Rule to relink during preinstall.
src/examples/reduction/CMakeFiles/reductionapp.dir/preinstall: src/examples/reduction/CMakeFiles/CMakeRelink.dir/reductionapp
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/preinstall

src/examples/reduction/CMakeFiles/reductionapp.dir/requires: src/examples/reduction/CMakeFiles/reductionapp.dir/reductionapp.cpp.o.requires
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/requires

src/examples/reduction/CMakeFiles/reductionapp.dir/clean:
	cd /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction && $(CMAKE_COMMAND) -P CMakeFiles/reductionapp.dir/cmake_clean.cmake
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/clean

src/examples/reduction/CMakeFiles/reductionapp.dir/depend:
	cd /home/elchaschab/devel/TankWar/GPUMLib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/elchaschab/devel/TankWar/GPUMLib /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction /home/elchaschab/devel/TankWar/GPUMLib /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction /home/elchaschab/devel/TankWar/GPUMLib/src/examples/reduction/CMakeFiles/reductionapp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/examples/reduction/CMakeFiles/reductionapp.dir/depend

