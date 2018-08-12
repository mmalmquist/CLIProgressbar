add_library (cpb progressbar.c ansi_ctrl.c)

target_include_directories (cpb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
