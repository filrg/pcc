cmake_minimum_required(VERSION 3.21)

project(pccExamples C)

include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcc REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcc::pcc)
  target_compile_features("${NAME}" PRIVATE c_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(encode)
add_example(decode)
add_example(multiple_encode)
add_example(multiple_decode)

if(EMSCRIPTEN)
    add_example(wasm_multiple_decode)
    set_target_properties(wasm_multiple_decode PROPERTIES
        OUTPUT_NAME "wasm_multiple_decode"
        SUFFIX ".js"
        )
    target_compile_options(wasm_multiple_decode PRIVATE
    )

    target_link_options(wasm_multiple_decode PRIVATE
        "-sMODULARIZE=1"
        "-sEXPORT_ES6=1"
        "-sEXPORT_NAME=createWasmDecoder"
        "-sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        "-sALLOW_MEMORY_GROWTH=1"
    )

endif()

add_folders(Example)
