# @author Yukio KANEDA

cmake_minimum_required(VERSION 3.1)

# == project
project(Pits CXX)

# == compile settings
if(MSVC)
    set(CMAKE_CXX_FLAGS "/std:c++17")
else()
    set(CMAKE_CXX_FLAGS "-std=c++17")
endif()
add_definitions(-DPITS_BUILD)

# == build
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.hpp")
add_library(Pits STATIC ${SOURCES} ${HEADERS})
target_link_libraries(Pits)

# == sub
add_subdirectory("test")

# == install
install(TARGETS Pits DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/Pits)

# Pits/include/Pits/...
# Pits/lib/...

# = MEMO
# == structured 
# if() elseif() else() endif()
# foreach() endforeach()
# while() endwhile()
# continue() break()
# function() endfunction()
# macro() endmacro()
# return()
# include() include_regular_expression() include_directories() include_guard()
# include_external_msproject()

# == definition
# set() unset()                          # embded environment
# add_definitions() remove_definitions() # compiler define
# add_compile_definitions()
# add_compile_options()

# == library option
# find_library()
# link_directories()
# link_libraries()

# == dependency
# add_library()
# add_executable()
# add_dependencies()

# == linker
# target_link_libraries()

# -- installation
# install()

# ----

# add_custom_command()

# add_subdirectory()
# aux_source_directory()

# build_command()

# cmake_host_system_information()
# cmake_minimum_required()
# cmake_parse_arguments()
# cmake_policy()
# get_cmake_property()

# configure_file()

# add_custom_target()
# set_target_properties() get_target_property()
# target_compile_definitions()
# target_compile_features()
# target_compile_options()
# target_include_directories()
# target_sources()

# == test
# create_test_sourcelist()
# ctest_...
# add_test()
# enable_testing()
# set_tests_properties() get_test_property()

# enable_language()
# export()

# == file manip
# file()
# find_file()
# find_package()
# find_path()
# find_program()

# fltk_wrap_ui()

# get_filename_component()


# list()
# load_cache()

# mark_as_advanced()
# math()

# message()

# option()

# project()

# == Qt
# qt_wrap_cpp()
# qt_wrap_ui()

# separate_arguments()

# define_property()
# set_property() get_property()
# set_directory_properties() get_directory_property()
# set_source_files_properties() get_source_file_property()

# site_name()
# source_group()
# string()

# try_compile()
# try_run()

# variable_watch()

# deprecated
#   install_files() install_programs() install_targets()
#   write_file() remove()
#   subdirs() make_directory()
#   exec_program()

# disallowed
#   subdir_depends()
#   variable_requires()
#   utility_source()
#   use_mangled_mesa()
#   output_required_files()
#   load_command()
#   export_library_dependencies()
#   build_name()





