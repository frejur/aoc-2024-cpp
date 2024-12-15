set(day_number 09)
message("Processing /day_${day_number}...")

set(T_THIS "aoc_2024_day_${day_number}")

add_executable(${T_THIS}
    day_09.h day_09.cpp
    part_01.cpp
    
    ../lib/dynbs.h
    ../lib/puzzle.h
    ../lib/puzzle.cpp
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

