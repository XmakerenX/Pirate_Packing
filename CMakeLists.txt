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
if (WIN32)
    find_package(boost)
else()
    find_package(Boost)
endif(WIN32)

include_directories(${Boost_INCLUDE_DIRS}) 

set(qttest_SRC  
  #GA:
  GA/Encoding/BinaryCreature.cpp
  GA/Encoding/DBLF_core.cpp
  GA/Encoding/PermutationCreature.cpp
  GA/Breeder.cpp
  GA/Configuration.cpp
  GA/GA_Random.cpp
  GA/GAThread.cpp
  GA/GA_Settings.cpp
  GA/GA_Core.cpp
  GA/Item.cpp
  #GUI:
  GUI/OpenGL/Camera.cpp
  GUI/OpenGL/FreeCam.cpp
  GUI/OpenGL/Mesh.cpp
  GUI/OpenGL/Object.cpp
  GUI/OpenGL/Solutionviewer.cpp
  GUI/ItemTableModel.cpp
  GUI/mainwindow.cpp
  #Include:
  includes/stdafx.cpp
)

# Tell CMake to create the executable
if (WIN32)
	#set(qttest_SRC ${qttest_SRC} resources/2D_Lego_Packing_Solver.rc)
	#set_source_files_properties(favicon.ico 2D_Lego_Packing_Solver.rc2 PROPERTIES LANGUAGE RC)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		add_executable(Pirate_Packing_gui ${qttest_SRC} mainGUI.cpp)
		add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
	else()
		add_executable(Pirate_Packing_gui ${qttest_SRC} mainGUI.cpp)
		add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
else()
	add_executable(Pirate_Packing_gui ${qttest_SRC} mainGUI.cpp)
	add_executable(Pirate_Packing_console ${qttest_SRC} mainConsole.cpp)
endif(WIN32)

# Use the Widgets module from Qt 5.
target_link_libraries(Pirate_Packing_gui Qt5::Widgets)
target_link_libraries(Pirate_Packing_console Qt5::Widgets)


# Copy resources folder to the build folder
add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Pirate_Packing_gui>/resources)
