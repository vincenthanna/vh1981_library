# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib

# Include any dependencies generated for this target.
include examples/CMakeFiles/example2.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/example2.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/example2.dir/flags.make

examples/CMakeFiles/example2.dir/example2.cpp.o: examples/CMakeFiles/example2.dir/flags.make
examples/CMakeFiles/example2.dir/example2.cpp.o: examples/example2.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object examples/CMakeFiles/example2.dir/example2.cpp.o"
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/example2.dir/example2.cpp.o -c /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples/example2.cpp

examples/CMakeFiles/example2.dir/example2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/example2.dir/example2.cpp.i"
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples/example2.cpp > CMakeFiles/example2.dir/example2.cpp.i

examples/CMakeFiles/example2.dir/example2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/example2.dir/example2.cpp.s"
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples/example2.cpp -o CMakeFiles/example2.dir/example2.cpp.s

examples/CMakeFiles/example2.dir/example2.cpp.o.requires:
.PHONY : examples/CMakeFiles/example2.dir/example2.cpp.o.requires

examples/CMakeFiles/example2.dir/example2.cpp.o.provides: examples/CMakeFiles/example2.dir/example2.cpp.o.requires
	$(MAKE) -f examples/CMakeFiles/example2.dir/build.make examples/CMakeFiles/example2.dir/example2.cpp.o.provides.build
.PHONY : examples/CMakeFiles/example2.dir/example2.cpp.o.provides

examples/CMakeFiles/example2.dir/example2.cpp.o.provides.build: examples/CMakeFiles/example2.dir/example2.cpp.o

# Object files for target example2
example2_OBJECTS = \
"CMakeFiles/example2.dir/example2.cpp.o"

# External object files for target example2
example2_EXTERNAL_OBJECTS =

examples/example2: examples/CMakeFiles/example2.dir/example2.cpp.o
examples/example2: examples/CMakeFiles/example2.dir/build.make
examples/example2: src/libjrtp.a
examples/example2: examples/CMakeFiles/example2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable example2"
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/example2.dir/build: examples/example2
.PHONY : examples/CMakeFiles/example2.dir/build

examples/CMakeFiles/example2.dir/requires: examples/CMakeFiles/example2.dir/example2.cpp.o.requires
.PHONY : examples/CMakeFiles/example2.dir/requires

examples/CMakeFiles/example2.dir/clean:
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples && $(CMAKE_COMMAND) -P CMakeFiles/example2.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/example2.dir/clean

examples/CMakeFiles/example2.dir/depend:
	cd /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples /snoopy/home/vincenthanna/workspace/git/vh1981_library/library/jrtplib/examples/CMakeFiles/example2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/example2.dir/depend

