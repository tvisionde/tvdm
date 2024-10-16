cmake_minimum_required (VERSION 3.29)

option( ADD_TINFOW "Add Tinfo to linking list (for Gentoo and possibly others). See Magiblot's Tvision's 11th issue." OFF )

project (tvdm
         VERSION 0.1.0
         LANGUAGES CXX
         DESCRIPTION "A TUI display manager.")

file (GLOB_RECURSE SRCODE "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_executable (tvdm ${SRCODE})

# configure_file (data/tvdm.desktop.in data/tvdm.desktop)

if (ADD_TINFOW)
    add_compile_options(-ltinfow)
endif ()

add_compile_options(-lncursesw -lpam -lgpm)

target_compile_features (tvdm PUBLIC cxx_std_17)
target_link_libraries (tvdm PUBLIC tvision)

install (TARGETS tvdm RUNTIME DESTINATION bin)