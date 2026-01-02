# Script to generate C++ header with embedded resources
# Usage: cmake -DINPUT_DIR=... -DOUTPUT_FILE=... -P GenerateResources.cmake

if(NOT INPUT_DIR OR NOT OUTPUT_FILE)
    message(FATAL_ERROR "Usage: cmake -DINPUT_DIR=... -DOUTPUT_FILE=... -P GenerateResources.cmake")
endif()

# Prepare the output file header
file(WRITE "${OUTPUT_FILE}" "#pragma once\n\n")
file(APPEND "${OUTPUT_FILE}" "#include <string>\n")
file(APPEND "${OUTPUT_FILE}" "#include <unordered_map>\n\n")
file(APPEND "${OUTPUT_FILE}" "// Auto-generated file. Do not edit directly.\n")
file(APPEND "${OUTPUT_FILE}" "const std::unordered_map<std::string, std::string> embedded_files = {\n")

# Find all HTML files in the input directory
file(GLOB HTML_FILES "${INPUT_DIR}/*.html")

foreach(FILE_PATH ${HTML_FILES})
    # Get filename (e.g. index.html)
    get_filename_component(FILE_NAME "${FILE_PATH}" NAME)

    # Read file content as hex strings (e.g. 616263...)
    file(READ "${FILE_PATH}" HEX_CONTENT HEX)

    # Convert hex to C++ string literal format (e.g. \x61\x62\x63...)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "\\\\x\\1" ESCAPED_CONTENT "${HEX_CONTENT}")

    # Write the entry to the map
    # We use "templates/${FILE_NAME}" as the key to match the expected path usage in code
    file(APPEND "${OUTPUT_FILE}" "    {\"templates/${FILE_NAME}\", \"${ESCAPED_CONTENT}\"},\n")
endforeach()

# Close the map definition
file(APPEND "${OUTPUT_FILE}" "};\n")
