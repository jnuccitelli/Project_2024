# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.30

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.30.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.30.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ArielaM/Desktop/Project_2024/MPI_Builds

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ArielaM/Desktop/Project_2024/MPI_Builds

# Include any dependencies generated for this target.
include CMakeFiles/helpers.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/helpers.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/helpers.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/helpers.dir/flags.make

CMakeFiles/helpers.dir/helpers.cpp.o: CMakeFiles/helpers.dir/flags.make
CMakeFiles/helpers.dir/helpers.cpp.o: helpers.cpp
CMakeFiles/helpers.dir/helpers.cpp.o: CMakeFiles/helpers.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/ArielaM/Desktop/Project_2024/MPI_Builds/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/helpers.dir/helpers.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/helpers.dir/helpers.cpp.o -MF CMakeFiles/helpers.dir/helpers.cpp.o.d -o CMakeFiles/helpers.dir/helpers.cpp.o -c /Users/ArielaM/Desktop/Project_2024/MPI_Builds/helpers.cpp

CMakeFiles/helpers.dir/helpers.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/helpers.dir/helpers.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ArielaM/Desktop/Project_2024/MPI_Builds/helpers.cpp > CMakeFiles/helpers.dir/helpers.cpp.i

CMakeFiles/helpers.dir/helpers.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/helpers.dir/helpers.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ArielaM/Desktop/Project_2024/MPI_Builds/helpers.cpp -o CMakeFiles/helpers.dir/helpers.cpp.s

# Object files for target helpers
helpers_OBJECTS = \
"CMakeFiles/helpers.dir/helpers.cpp.o"

# External object files for target helpers
helpers_EXTERNAL_OBJECTS =

helpers: CMakeFiles/helpers.dir/helpers.cpp.o
helpers: CMakeFiles/helpers.dir/build.make
helpers: CMakeFiles/helpers.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/ArielaM/Desktop/Project_2024/MPI_Builds/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable helpers"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/helpers.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/helpers.dir/build: helpers
.PHONY : CMakeFiles/helpers.dir/build

CMakeFiles/helpers.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/helpers.dir/cmake_clean.cmake
.PHONY : CMakeFiles/helpers.dir/clean

CMakeFiles/helpers.dir/depend:
	cd /Users/ArielaM/Desktop/Project_2024/MPI_Builds && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ArielaM/Desktop/Project_2024/MPI_Builds /Users/ArielaM/Desktop/Project_2024/MPI_Builds /Users/ArielaM/Desktop/Project_2024/MPI_Builds /Users/ArielaM/Desktop/Project_2024/MPI_Builds /Users/ArielaM/Desktop/Project_2024/MPI_Builds/CMakeFiles/helpers.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/helpers.dir/depend

