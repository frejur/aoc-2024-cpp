cmake_minimum_required(VERSION 3.2)
project(aoc_2024 VERSION 0.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL "ALL") # When `SELECTED_TARGET` is set to this value, all targets
               # will be built. Do NOT insert this value into the
               # `TARGETS` list below

# Shorthand for appending to TARGETS and DIRS
function(apd list_var)
   foreach(target IN LISTS ARGN)
        list(APPEND ${list_var} "${target}")
    endforeach()
    # Return changes made to local copy to Parent scope
    set(${list_var} ${${list_var}} PARENT_SCOPE)
endfunction()

# Targets and directories
set(TARGETS "Day 1"  "Day 2"  "Day 3"  "Day 4"  "Day 5"  "Day 6")
set(DIRS    "day_01" "day_02" "day_03" "day_04" "day_05" "day_06")

apd(TARGETS "Day 7"  "Day 8"  "Day 9"  "Day 10" "Day 11" "Day 12")
apd(DIRS    "day_07" "day_08" "day_09" "day_10" "day_11" "day_12")

apd(TARGETS "Day 13" "Day 14")
apd(DIRS    "day_13" "day_14")

# Ensure TARGETS length == DIRS length
list(LENGTH TARGETS num_targets)
list(LENGTH DIRS num_dirs)
if(NOT num_targets EQUAL num_dirs)
    message(FATAL_ERROR "Number of entries in TARGETS (${num_targets})"
                        "does not match number of entries in "
                        "DIRS (${num_dirs})")
endif()

# Default target
list(GET TARGETS 0 DEFAULT_TARGET)

# Make targets selectable in `ccmake` and similar
set(SELECTED_TARGET ${DEFAULT_TARGET} CACHE STRING "Selected target")
set_property(CACHE SELECTED_TARGET PROPERTY STRINGS ${test_name} ${TARGETS} ${ALL})

# Print targets
message(STATUS "Available values for SELECTED_TARGET:")
foreach(target ${test_name} ${TARGETS} ${ALL})
    message(STATUS " ${target}")
endforeach()
message(STATUS "Currently selected target: ${SELECTED_TARGET}")

# Build 'All targets' if selected
if(${SELECTED_TARGET} STREQUAL ${ALL})
    message(STATUS "Building all targets...")
    foreach(target_dir ${DIRS})
        add_subdirectory(${CMAKE_SOURCE_DIR}/${target_dir})
    endforeach()
else()

# Otherwise, build 'Selected target'
    math(EXPR max_idx "${num_targets} - 1")
    foreach(i RANGE ${max_idx})
        list(GET TARGETS ${i} target)
        list(GET DIRS ${i} target_dir)

        if(${SELECTED_TARGET} STREQUAL ${target})
            message(STATUS "Building ${target}...")
            add_subdirectory(${CMAKE_SOURCE_DIR}/${target_dir})
            break()
        endif()
    endforeach()
endif()

