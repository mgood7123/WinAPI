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
CMAKE_SOURCE_DIR = /home/smallville7123/app/WinAPI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/smallville7123/app/WinAPI/debug

# Include any dependencies generated for this target.
include Windows/threads/CMakeFiles/println.dir/depend.make

# Include the progress variables for this target.
include Windows/threads/CMakeFiles/println.dir/progress.make

# Include the compile flags for this target's objects.
include Windows/threads/CMakeFiles/println.dir/flags.make

Windows/threads/CMakeFiles/println.dir/include/println.cpp.o: Windows/threads/CMakeFiles/println.dir/flags.make
Windows/threads/CMakeFiles/println.dir/include/println.cpp.o: ../Windows/threads/include/println.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/app/WinAPI/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Windows/threads/CMakeFiles/println.dir/include/println.cpp.o"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/println.dir/include/println.cpp.o -c /home/smallville7123/app/WinAPI/Windows/threads/include/println.cpp

Windows/threads/CMakeFiles/println.dir/include/println.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/println.dir/include/println.cpp.i"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/app/WinAPI/Windows/threads/include/println.cpp > CMakeFiles/println.dir/include/println.cpp.i

Windows/threads/CMakeFiles/println.dir/include/println.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/println.dir/include/println.cpp.s"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/app/WinAPI/Windows/threads/include/println.cpp -o CMakeFiles/println.dir/include/println.cpp.s

# Object files for target println
println_OBJECTS = \
"CMakeFiles/println.dir/include/println.cpp.o"

# External object files for target println
println_EXTERNAL_OBJECTS =

Windows/threads/libprintln.so: Windows/threads/CMakeFiles/println.dir/include/println.cpp.o
Windows/threads/libprintln.so: Windows/threads/CMakeFiles/println.dir/build.make
Windows/threads/libprintln.so: Windows/threads/CMakeFiles/println.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/app/WinAPI/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libprintln.so"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/println.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Windows/threads/CMakeFiles/println.dir/build: Windows/threads/libprintln.so

.PHONY : Windows/threads/CMakeFiles/println.dir/build

Windows/threads/CMakeFiles/println.dir/clean:
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads && $(CMAKE_COMMAND) -P CMakeFiles/println.dir/cmake_clean.cmake
.PHONY : Windows/threads/CMakeFiles/println.dir/clean

Windows/threads/CMakeFiles/println.dir/depend:
	cd /home/smallville7123/app/WinAPI/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/app/WinAPI /home/smallville7123/app/WinAPI/Windows/threads /home/smallville7123/app/WinAPI/debug /home/smallville7123/app/WinAPI/debug/Windows/threads /home/smallville7123/app/WinAPI/debug/Windows/threads/CMakeFiles/println.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Windows/threads/CMakeFiles/println.dir/depend

