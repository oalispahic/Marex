# Smoke tests: every example program must run and print exactly what
# tests/expected/<name>.out contains.

set(EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/test_program)

function(marex_example_test name)
    add_test(NAME example.${name}
            COMMAND ${CMAKE_COMMAND}
            -DMAREX=$<TARGET_FILE:marex>
            -DSCRIPT=${EXAMPLE_DIR}/${name}.mx
            -DEXPECTED=${CMAKE_CURRENT_SOURCE_DIR}/expected/${name}.txt
            "-DARGS=${ARGN}"
            -P ${CMAKE_CURRENT_SOURCE_DIR}/run_example.cmake)
endfunction()

marex_example_test(fibb)
marex_example_test(fact)
marex_example_test(example)
marex_example_test(loops)
marex_example_test(types)
marex_example_test(args one two)

# CLI behaviour

add_test(NAME cli.version COMMAND marex --version)
set_tests_properties(cli.version PROPERTIES PASS_REGULAR_EXPRESSION "^Marex ${PROJECT_VERSION}\n$")

add_test(NAME cli.help COMMAND marex --help)
set_tests_properties(cli.help PROPERTIES PASS_REGULAR_EXPRESSION "Usage: marex")

add_test(NAME cli.unknown_option COMMAND marex --bogus)
set_tests_properties(cli.unknown_option PROPERTIES WILL_FAIL TRUE)

add_test(NAME cli.missing_file COMMAND marex ${EXAMPLE_DIR}/does_not_exist.mx)
set_tests_properties(cli.missing_file PROPERTIES WILL_FAIL TRUE)

add_test(NAME cli.tokens COMMAND marex --tokens ${EXAMPLE_DIR}/fact.mx)
set_tests_properties(cli.tokens PROPERTIES PASS_REGULAR_EXPRESSION "VAR.*faktorijel.*END_OF_FILE")
