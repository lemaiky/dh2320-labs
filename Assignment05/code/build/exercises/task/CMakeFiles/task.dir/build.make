# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build

# Include any dependencies generated for this target.
include exercises/task/CMakeFiles/task.dir/depend.make

# Include the progress variables for this target.
include exercises/task/CMakeFiles/task.dir/progress.make

# Include the compile flags for this target's objects.
include exercises/task/CMakeFiles/task.dir/flags.make

exercises/task/CMakeFiles/task.dir/foundation.cpp.o: exercises/task/CMakeFiles/task.dir/flags.make
exercises/task/CMakeFiles/task.dir/foundation.cpp.o: ../exercises/task/foundation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object exercises/task/CMakeFiles/task.dir/foundation.cpp.o"
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task && /Library/Developer/CommandLineTools/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/task.dir/foundation.cpp.o -c /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/exercises/task/foundation.cpp

exercises/task/CMakeFiles/task.dir/foundation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/task.dir/foundation.cpp.i"
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task && /Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/exercises/task/foundation.cpp > CMakeFiles/task.dir/foundation.cpp.i

exercises/task/CMakeFiles/task.dir/foundation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/task.dir/foundation.cpp.s"
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task && /Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/exercises/task/foundation.cpp -o CMakeFiles/task.dir/foundation.cpp.s

exercises/task/CMakeFiles/task.dir/foundation.cpp.o.requires:

.PHONY : exercises/task/CMakeFiles/task.dir/foundation.cpp.o.requires

exercises/task/CMakeFiles/task.dir/foundation.cpp.o.provides: exercises/task/CMakeFiles/task.dir/foundation.cpp.o.requires
	$(MAKE) -f exercises/task/CMakeFiles/task.dir/build.make exercises/task/CMakeFiles/task.dir/foundation.cpp.o.provides.build
.PHONY : exercises/task/CMakeFiles/task.dir/foundation.cpp.o.provides

exercises/task/CMakeFiles/task.dir/foundation.cpp.o.provides.build: exercises/task/CMakeFiles/task.dir/foundation.cpp.o


# Object files for target task
task_OBJECTS = \
"CMakeFiles/task.dir/foundation.cpp.o"

# External object files for target task
task_EXTERNAL_OBJECTS =

task: exercises/task/CMakeFiles/task.dir/foundation.cpp.o
task: exercises/task/CMakeFiles/task.dir/build.make
task: libglew.dylib
task: libglfw.dylib
task: libopengl.dylib
task: libglew.dylib
task: libglfw.dylib
task: libraytracer.dylib
task: exercises/task/CMakeFiles/task.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../task"
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/task.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
exercises/task/CMakeFiles/task.dir/build: task

.PHONY : exercises/task/CMakeFiles/task.dir/build

exercises/task/CMakeFiles/task.dir/requires: exercises/task/CMakeFiles/task.dir/foundation.cpp.o.requires

.PHONY : exercises/task/CMakeFiles/task.dir/requires

exercises/task/CMakeFiles/task.dir/clean:
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task && $(CMAKE_COMMAND) -P CMakeFiles/task.dir/cmake_clean.cmake
.PHONY : exercises/task/CMakeFiles/task.dir/clean

exercises/task/CMakeFiles/task.dir/depend:
	cd /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/exercises/task /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task /Users/Maiky/Dropbox/KTH/DH2320/Assignments/dh2320-labs/Assignment05/code/build/exercises/task/CMakeFiles/task.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : exercises/task/CMakeFiles/task.dir/depend

