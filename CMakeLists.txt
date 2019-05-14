cmake_minimum_required(VERSION 3.0)
project(Pirate_Packing)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)

set(GA_LIBRARY_NAME "GALib")
set(GUI_LIBRARY_NAME "GUILib")

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
if (WIN32)
	find_package(Boost)
	link_directories($ENV{BOOST_LIBRARYDIR})
else()
    find_package(Boost REQUIRED COMPONENTS iostreams)
endif(WIN32)
find_package(Threads QUIET REQUIRED)

include_directories(${Boost_INCLUDE_DIRS}) 

set(GA_SRC  
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
  #Include:
  includes/stdafx.cpp
)

set(GUI_SRC  
  #GUI:
  GUI/OpenGL/Camera.cpp
  GUI/OpenGL/FreeCam.cpp
  GUI/OpenGL/Mesh.cpp
  GUI/OpenGL/Object.cpp
  GUI/OpenGL/Solutionviewer.cpp
  GUI/ItemTableModel.cpp
)

set(MAINGUI_SRC
    GUI/mainwindow.cpp
    mainGUI.cpp)

set(VIEWER_SRC
    GUI/viewerOnlyWindow.cpp
    mainViewer.cpp)

set(DBLFDEBUG_SRC
    GUI/dblfDebugWindow.cpp
    mainDBLFDebug.cpp)
    
add_library(${GA_LIBRARY_NAME} ${GA_SRC})
add_library(${GUI_LIBRARY_NAME} ${GUI_SRC})

# Tell CMake to create the executable
if (WIN32)
	#set(qttest_SRC ${qttest_SRC} resources/2D_Lego_Packing_Solver.rc)
	#set_source_files_properties(favicon.ico 2D_Lego_Packing_Solver.rc2 PROPERTIES LANGUAGE RC)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		add_executable(Pirate_Packing_gui ${MAINGUI_SRC})
		add_executable(Pirate_Packing_viewer ${VIEWER_SRC})
		add_executable(Pirate_Packing_dblf ${DBLFDEBUG_SRC})
		add_executable(Pirate_Packing_console mainConsole.cpp)
	else()
		add_executable(Pirate_Packing_gui ${MAINGUI_SRC})
		add_executable(Pirate_Packing_viewer ${VIEWER_SRC})
		add_executable(Pirate_Packing_dblf ${DBLFDEBUG_SRC})
		add_executable(Pirate_Packing_console mainConsole.cpp)
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
else()
	add_executable(Pirate_Packing_gui ${MAINGUI_SRC})
	add_executable(Pirate_Packing_viewer ${VIEWER_SRC})
	add_executable(Pirate_Packing_dblf ${DBLFDEBUG_SRC})
	add_executable(Pirate_Packing_console mainConsole.cpp)
	target_link_libraries(Pirate_Packing_console Boost::iostreams)
endif(WIN32)

# Use the Widgets module from Qt 5.
target_link_libraries(${GA_LIBRARY_NAME} Qt5::Widgets ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${GUI_LIBRARY_NAME} ${GA_LIBRARY_NAME})
target_link_libraries(Pirate_Packing_gui ${GUI_LIBRARY_NAME})
target_link_libraries(Pirate_Packing_viewer ${GUI_LIBRARY_NAME})
target_link_libraries(Pirate_Packing_dblf ${GUI_LIBRARY_NAME})
target_link_libraries(Pirate_Packing_console ${GA_LIBRARY_NAME})

# Copy resources folder to the build folder
add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Pirate_Packing_gui>/resources)

# Copy Qt dlls for windows executable
if(WIN32)
	if (CMAKE_BUILD_TYPE MATCHES Debug)
		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}bin/Qt5Cored.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)

		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}bin/Qt5Guid.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)

		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Widgetsd.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)
	else()
		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}bin/Qt5Core.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)

		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}bin/Qt5Gui.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)

		add_custom_command(TARGET Pirate_Packing_gui POST_BUILD
						   COMMAND ${CMAKE_COMMAND} -E copy
						   $ENV{CMAKE_PREFIX_PATH}/bin/Qt5Widgets.dll $<TARGET_FILE_DIR:Pirate_Packing_gui>)
	endif(CMAKE_BUILD_TYPE MATCHES Debug)
endif(WIN32)
