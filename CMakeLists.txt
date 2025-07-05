cmake_minimum_required(VERSION 3.10)
project(contact_manager)

# Find GTK4 package
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Add executable
add_executable(contact_manager main.c)

# Link GTK4
target_link_directories(contact_manager PRIVATE ${GTK4_LIBRARY_DIRS})
target_include_directories(contact_manager PRIVATE ${GTK4_INCLUDE_DIRS})
target_link_libraries(contact_manager ${GTK4_LIBRARIES})

# Add compiler flags
target_compile_options(contact_manager PRIVATE ${GTK4_CFLAGS_OTHER}) 