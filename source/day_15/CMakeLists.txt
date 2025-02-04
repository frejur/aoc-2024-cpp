set(day_number 15)
message("Processing /day_${day_number}...")

set(T_THIS "aoc_2024_day_${day_number}")

add_executable(${T_THIS}
    day_15.cpp 
    io_utils.h io_utils.cpp
    grid_utils.h grid_utils.cpp
    warehouse.h warehouse.cpp
    warehouse_col.h warehouse_col.cpp
    robot.h robot.cpp
    box.h box.cpp
    move.h move.cpp
    keys.h keys.cpp
    
    r_box.h r_box.cpp
    
    # Part one
    s_box.h s_box.cpp
    
    # Part two
    wd_box.h wd_box.cpp
    
    ../lib/direction.h
    ../lib/direction.cpp
    ../lib/vec2d.h
    ../lib/vec2d.cpp
    ../lib/puzzle.h
    ../lib/puzzle.cpp
    ../lib/grid.h
    ../lib/grid.cpp
    ../lib/dynbs.h
    ../lib/bit_grid.h
    ../lib/bit_grid.cpp
    ../lib/extract.h
    ../lib/extract.cpp
    edge_tile.h edge_tile.cpp
)

# Dynamically copy the puzzle sample input into a
# sub directory of the executable

# Sample Input
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${T_THIS}>/input
)
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../../input/202412${day_number}_input.txt
        $<TARGET_FILE_DIR:${T_THIS}>/input/202412${day_number}_input.txt
)

# Test Input
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${T_THIS}>/test
)
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../../test/202412${day_number}_test.txt
        $<TARGET_FILE_DIR:${T_THIS}>/test/202412${day_number}_test.txt
)

# Test answers
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../../test/secret.txt
        $<TARGET_FILE_DIR:${T_THIS}>/test/secret.txt
)

