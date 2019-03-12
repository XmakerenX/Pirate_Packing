cmake_minimum_required(VERSION 3.0)
project(2D_Lego_Packing_Solver)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

set(qttest_SRC  
  #GA:
  2D_Lego_Packing_Solver/GA/Breeder.cpp
  2D_Lego_Packing_Solver/GA/Configuration.cpp
  2D_Lego_Packing_Solver/GA/Creature.cpp
  2D_Lego_Packing_Solver/GA/GA_Random.cpp
  2D_Lego_Packing_Solver/GA/GAThread.cpp
  2D_Lego_Packing_Solver/GA/GraphBin.cpp
  2D_Lego_Packing_Solver/GA/Item.cpp

  #GUI:
  2D_Lego_Packing_Solver/GUI/mainwindow.cpp
  
  #Include:
  2D_Lego_Packing_Solver/includes/stdafx.cpp
)

# Tell CMake to create the executable
if (WIN32)
	set(qttest_SRC ${qttest_SRC} 2D_Lego_Packing_Solver/resources/2D_Lego_Packing_Solver.rc)
	set_source_files_properties(favicon.ico 2D_Lego_Packing_Solver.rc2 PROPERTIES LANGUAGE RC)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		add_executable(2D_Lego_Packing_Solver_gui ${qttest_SRC} 2D_Lego_Packing_Solver/mainGUI.cpp)
		add_executable(2D_Lego_Packing_Solver_console ${qttest_SRC} 2D_Lego_Packing_Solver/mainConsole.cpp)
	else()
		add_executable(2D_Lego_Packing_Solver_gui WIN32 ${qttest_SRC} 2D_Lego_Packing_Solver/mainGUI.cpp)
		add_executable(2D_Lego_Packing_Solver_console ${qttest_SRC} 2D_Lego_Packing_Solver/mainConsole.cpp)
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
else()
	add_executable(2D_Lego_Packing_Solver_gui ${qttest_SRC} 2D_Lego_Packing_Solver/mainGUI.cpp)
	add_executable(2D_Lego_Packing_Solver_console ${qttest_SRC} 2D_Lego_Packing_Solver/mainConsole.cpp)
endif(WIN32)

# Use the Widgets module from Qt 5.
target_link_libraries(2D_Lego_Packing_Solver_gui Qt5::Widgets)
 
