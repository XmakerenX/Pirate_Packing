cmake_minimum_required(VERSION 3.0)
project(Pirate_Packing)

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
  GA/Encoding/BinaryEncoding.cpp
  GA/Encoding/PermutationEncoding.cpp
  GA/Breeder.cpp
  GA/BreederNew.cpp
  GA/Configuration.cpp
  GA/Creature.cpp
  GA/CreatureNew.cpp
  GA/GA_Random.cpp
  GA/GAThread.cpp
  GA/GraphBin.cpp
  GA/Item.cpp
  #GUI:
  GUI/mainwindow.cpp
  #Include:
  includes/stdafx.cpp
)

# Tell CMake to create the executable
if (WIN32)
	set(qttest_SRC ${qttest_SRC} resources/2D_Lego_Packing_Solver.rc)
	set_source_files_properties(favicon.ico 2D_Lego_Packing_Solver.rc2 PROPERTIES LANGUAGE RC)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		add_executable(Pirate_Packing_gui ${qttest_SRC} mainGUI.cpp)
		add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
	else()
		add_executable(Pirate_Packing_gui WIN32 ${qttest_SRC} mainGUI.cpp)
		add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
else()
	add_executable(Pirate_Packing_gui ${qttest_SRC} mainGUI.cpp)
	add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
endif(WIN32)

# Use the Widgets module from Qt 5.
target_link_libraries(Pirate_Packing_gui Qt5::Widgets)
target_link_libraries(Pirate_Packing_console Qt5::Widgets)
 
