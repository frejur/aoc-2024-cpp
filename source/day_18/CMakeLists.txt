set(day_number 18)
message("Processing /day_${day_number}...")

set(T_THIS "aoc_2024_day_${day_number}")

add_executable(${T_THIS}
    day_18.cpp 
    io.h io.cpp
    bytegrid.h bytegrid.cpp
    keys.h keys.cpp

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
    point.h point.cpp
    solve.h solve.cpp
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

