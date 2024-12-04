set(day_number 01)
message("Processing /day_${day_number}...")

set(T_THIS "aoc_2024_day_${day_number}")

add_executable(${T_THIS}
    day_01.h day_01.cpp
    part_01.cpp
    part_02.cpp
    
    ../lib/puzzle.h
    ../lib/puzzle.cpp
)

# Dynamically copy the puzzle sample input into a
# sub directory of the executable
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${T_THIS}>/input
)
add_custom_command(TARGET ${T_THIS} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/../../input/202412${day_number}_input.txt
        $<TARGET_FILE_DIR:${T_THIS}>/input/202412${day_number}_input.txt
)

