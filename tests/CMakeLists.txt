# Smoke tests: every example program must run and print exactly what
# tests/expected/<name>.out contains.

set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/test_program)

function(marex_example_test name)
    add_test(NAME example.${name}
            COMMAND ${CMAKE_COMMAND}
            -DMAREX=$<TARGET_FILE:marex>
            -DSCRIPT=${EXAMPLE_DIR}/${name}.mx
            -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/expected/${name}.txt
            -DARGS=${ARGN}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/run_example.cmake)
endfunction()

marex_example_test(fibb)
marex_example_test(fact)
