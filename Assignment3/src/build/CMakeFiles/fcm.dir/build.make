# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rafael/Desktop/IC/IC/Assignment3/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rafael/Desktop/IC/IC/Assignment3/src/build

# Include any dependencies generated for this target.
include CMakeFiles/fcm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/fcm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/fcm.dir/flags.make

CMakeFiles/fcm.dir/fcm.cpp.o: CMakeFiles/fcm.dir/flags.make
CMakeFiles/fcm.dir/fcm.cpp.o: ../fcm.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rafael/Desktop/IC/IC/Assignment3/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/fcm.dir/fcm.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/fcm.dir/fcm.cpp.o -c /home/rafael/Desktop/IC/IC/Assignment3/src/fcm.cpp

CMakeFiles/fcm.dir/fcm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/fcm.dir/fcm.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rafael/Desktop/IC/IC/Assignment3/src/fcm.cpp > CMakeFiles/fcm.dir/fcm.cpp.i

CMakeFiles/fcm.dir/fcm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/fcm.dir/fcm.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rafael/Desktop/IC/IC/Assignment3/src/fcm.cpp -o CMakeFiles/fcm.dir/fcm.cpp.s

# Object files for target fcm
fcm_OBJECTS = \
"CMakeFiles/fcm.dir/fcm.cpp.o"

# External object files for target fcm
fcm_EXTERNAL_OBJECTS =

/home/rafael/Desktop/IC/IC/Assignment3/bin/fcm: CMakeFiles/fcm.dir/fcm.cpp.o
/home/rafael/Desktop/IC/IC/Assignment3/bin/fcm: CMakeFiles/fcm.dir/build.make
/home/rafael/Desktop/IC/IC/Assignment3/bin/fcm: CMakeFiles/fcm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rafael/Desktop/IC/IC/Assignment3/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/rafael/Desktop/IC/IC/Assignment3/bin/fcm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fcm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/fcm.dir/build: /home/rafael/Desktop/IC/IC/Assignment3/bin/fcm

.PHONY : CMakeFiles/fcm.dir/build

CMakeFiles/fcm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/fcm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/fcm.dir/clean

CMakeFiles/fcm.dir/depend:
	cd /home/rafael/Desktop/IC/IC/Assignment3/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rafael/Desktop/IC/IC/Assignment3/src /home/rafael/Desktop/IC/IC/Assignment3/src /home/rafael/Desktop/IC/IC/Assignment3/src/build /home/rafael/Desktop/IC/IC/Assignment3/src/build /home/rafael/Desktop/IC/IC/Assignment3/src/build/CMakeFiles/fcm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/fcm.dir/depend

