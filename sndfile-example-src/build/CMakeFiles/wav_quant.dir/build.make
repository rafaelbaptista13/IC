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
CMAKE_SOURCE_DIR = "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build"

# Include any dependencies generated for this target.
include CMakeFiles/wav_quant.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/wav_quant.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/wav_quant.dir/flags.make

CMakeFiles/wav_quant.dir/wav_quant.cpp.o: CMakeFiles/wav_quant.dir/flags.make
CMakeFiles/wav_quant.dir/wav_quant.cpp.o: ../wav_quant.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/wav_quant.dir/wav_quant.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/wav_quant.dir/wav_quant.cpp.o -c "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/wav_quant.cpp"

CMakeFiles/wav_quant.dir/wav_quant.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/wav_quant.dir/wav_quant.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/wav_quant.cpp" > CMakeFiles/wav_quant.dir/wav_quant.cpp.i

CMakeFiles/wav_quant.dir/wav_quant.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/wav_quant.dir/wav_quant.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/wav_quant.cpp" -o CMakeFiles/wav_quant.dir/wav_quant.cpp.s

# Object files for target wav_quant
wav_quant_OBJECTS = \
"CMakeFiles/wav_quant.dir/wav_quant.cpp.o"

# External object files for target wav_quant
wav_quant_EXTERNAL_OBJECTS =

/home/diogo/Desktop/Uni/5º\ Ano/IC/sndfile-example-bin/wav_quant: CMakeFiles/wav_quant.dir/wav_quant.cpp.o
/home/diogo/Desktop/Uni/5º\ Ano/IC/sndfile-example-bin/wav_quant: CMakeFiles/wav_quant.dir/build.make
/home/diogo/Desktop/Uni/5º\ Ano/IC/sndfile-example-bin/wav_quant: CMakeFiles/wav_quant.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable \"/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-bin/wav_quant\""
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wav_quant.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/wav_quant.dir/build: /home/diogo/Desktop/Uni/5º\ Ano/IC/sndfile-example-bin/wav_quant

.PHONY : CMakeFiles/wav_quant.dir/build

CMakeFiles/wav_quant.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/wav_quant.dir/cmake_clean.cmake
.PHONY : CMakeFiles/wav_quant.dir/clean

CMakeFiles/wav_quant.dir/depend:
	cd "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src" "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src" "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build" "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build" "/home/diogo/Desktop/Uni/5º Ano/IC/sndfile-example-src/build/CMakeFiles/wav_quant.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/wav_quant.dir/depend

