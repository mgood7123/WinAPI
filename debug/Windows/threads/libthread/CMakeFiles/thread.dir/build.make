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
include Windows/threads/libthread/CMakeFiles/thread.dir/depend.make

# Include the progress variables for this target.
include Windows/threads/libthread/CMakeFiles/thread.dir/progress.make

# Include the compile flags for this target's objects.
include Windows/threads/libthread/CMakeFiles/thread.dir/flags.make

Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o: Windows/threads/libthread/CMakeFiles/thread.dir/flags.make
Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o: ../Windows/threads/libthread/src/ExecutionManager.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/smallville7123/app/WinAPI/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/thread.dir/src/ExecutionManager.cpp.o -c /home/smallville7123/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp

Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/thread.dir/src/ExecutionManager.cpp.i"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/smallville7123/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp > CMakeFiles/thread.dir/src/ExecutionManager.cpp.i

Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/thread.dir/src/ExecutionManager.cpp.s"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/smallville7123/app/WinAPI/Windows/threads/libthread/src/ExecutionManager.cpp -o CMakeFiles/thread.dir/src/ExecutionManager.cpp.s

# Object files for target thread
thread_OBJECTS = \
"CMakeFiles/thread.dir/src/ExecutionManager.cpp.o"

# External object files for target thread
thread_EXTERNAL_OBJECTS =

Windows/threads/libthread/libthread.so: Windows/threads/libthread/CMakeFiles/thread.dir/src/ExecutionManager.cpp.o
Windows/threads/libthread/libthread.so: Windows/threads/libthread/CMakeFiles/thread.dir/build.make
Windows/threads/libthread/libthread.so: Windows/threads/libstack/libstack.so
Windows/threads/libthread/libthread.so: Windows/threads/libthread/CMakeFiles/thread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/smallville7123/app/WinAPI/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libthread.so"
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/thread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Windows/threads/libthread/CMakeFiles/thread.dir/build: Windows/threads/libthread/libthread.so

.PHONY : Windows/threads/libthread/CMakeFiles/thread.dir/build

Windows/threads/libthread/CMakeFiles/thread.dir/clean:
	cd /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread && $(CMAKE_COMMAND) -P CMakeFiles/thread.dir/cmake_clean.cmake
.PHONY : Windows/threads/libthread/CMakeFiles/thread.dir/clean

Windows/threads/libthread/CMakeFiles/thread.dir/depend:
	cd /home/smallville7123/app/WinAPI/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smallville7123/app/WinAPI /home/smallville7123/app/WinAPI/Windows/threads/libthread /home/smallville7123/app/WinAPI/debug /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread /home/smallville7123/app/WinAPI/debug/Windows/threads/libthread/CMakeFiles/thread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Windows/threads/libthread/CMakeFiles/thread.dir/depend

