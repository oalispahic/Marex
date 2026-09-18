# Runs one Marex program and compares its stdout with an expected file.
#
#   cmake -DMAREX=<binary> -DSCRIPT=<program.mx> -DEXPECTED=<expected.out>
#         [-DARGS="a;b;c"] -P run_example.cmake

if(NOT MAREX OR NOT SCRIPT OR NOT EXPECTED)
    message(FATAL_ERROR "MAREX, SCRIPT and EXPECTED must be set")
endif()

execute_process(
        COMMAND ${MAREX} ${SCRIPT} ${ARGS}
        OUTPUT_VARIABLE actual
        ERROR_VARIABLE errors
        RESULT_VARIABLE status
)

if(NOT status EQUAL 0)
    message(FATAL_ERROR "${SCRIPT} exited with ${status}\n${errors}")
endif()

file(READ ${EXPECTED} expected)

if(NOT actual STREQUAL expected)
    message(FATAL_ERROR
            "Output of ${SCRIPT} differs from ${EXPECTED}\n"
            "--- expected ---\n${expected}\n--- actual ---\n${actual}\n--- stderr ---\n${errors}")
endif()
