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
CMAKE_SOURCE_DIR = /home/rafael/Desktop/IC/IC/Assignment2/opencv-src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rafael/Desktop/IC/IC/Assignment2/opencv-src

# Include any dependencies generated for this target.
include CMakeFiles/cp_image.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cp_image.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cp_image.dir/flags.make

CMakeFiles/cp_image.dir/cp_image.cpp.o: CMakeFiles/cp_image.dir/flags.make
CMakeFiles/cp_image.dir/cp_image.cpp.o: cp_image.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rafael/Desktop/IC/IC/Assignment2/opencv-src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cp_image.dir/cp_image.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cp_image.dir/cp_image.cpp.o -c /home/rafael/Desktop/IC/IC/Assignment2/opencv-src/cp_image.cpp

CMakeFiles/cp_image.dir/cp_image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cp_image.dir/cp_image.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rafael/Desktop/IC/IC/Assignment2/opencv-src/cp_image.cpp > CMakeFiles/cp_image.dir/cp_image.cpp.i

CMakeFiles/cp_image.dir/cp_image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cp_image.dir/cp_image.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rafael/Desktop/IC/IC/Assignment2/opencv-src/cp_image.cpp -o CMakeFiles/cp_image.dir/cp_image.cpp.s

# Object files for target cp_image
cp_image_OBJECTS = \
"CMakeFiles/cp_image.dir/cp_image.cpp.o"

# External object files for target cp_image
cp_image_EXTERNAL_OBJECTS =

/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: CMakeFiles/cp_image.dir/cp_image.cpp.o
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: CMakeFiles/cp_image.dir/build.make
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_aruco.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_bgsegm.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_bioinspired.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_ccalib.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_dnn_objdetect.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_dnn_superres.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_dpm.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_face.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_freetype.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_fuzzy.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_hdf.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_hfs.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_img_hash.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_line_descriptor.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_quality.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_reg.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_rgbd.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_saliency.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_shape.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_stereo.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_structured_light.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_surface_matching.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_tracking.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_viz.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_xobjdetect.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_xphoto.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_datasets.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_plot.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_text.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_dnn.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_phase_unwrapping.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_optflow.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_ximgproc.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_video.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_videoio.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: /usr/lib/x86_64-linux-gnu/libopencv_core.so.4.2.0
/home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image: CMakeFiles/cp_image.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rafael/Desktop/IC/IC/Assignment2/opencv-src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cp_image.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cp_image.dir/build: /home/rafael/Desktop/IC/IC/Assignment2/opencv-bin/cp_image

.PHONY : CMakeFiles/cp_image.dir/build

CMakeFiles/cp_image.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cp_image.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cp_image.dir/clean

CMakeFiles/cp_image.dir/depend:
	cd /home/rafael/Desktop/IC/IC/Assignment2/opencv-src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rafael/Desktop/IC/IC/Assignment2/opencv-src /home/rafael/Desktop/IC/IC/Assignment2/opencv-src /home/rafael/Desktop/IC/IC/Assignment2/opencv-src /home/rafael/Desktop/IC/IC/Assignment2/opencv-src /home/rafael/Desktop/IC/IC/Assignment2/opencv-src/CMakeFiles/cp_image.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cp_image.dir/depend

