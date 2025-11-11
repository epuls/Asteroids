cmake_minimum_required(VERSION 3.27)
project(Asteroids)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# prevent installing to system directories.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

set(SDLTTF_VENDORED ON)

#include(FetchContent)

add_library(glad STATIC ${PROJECT_SOURCE_DIR}/extern/glad/src/glad.c)
target_include_directories(glad PUBLIC ${PROJECT_SOURCE_DIR}/extern/glad/include/)

if ((APPLE AND NOT CMAKE_SYSTEM_NAME MATCHES "Darwin") OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")    # Disable shared builds on platforms where it does not make sense to use them
    set(SDL_SHARED OFF)
else()
    set(SDL_SHARED ON)
endif()

if(MSVC)
    if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)				# parallelize each target, unless Ninja is the generator
    endif()
endif()

set(EXECUTABLE_NAME ${PROJECT_NAME})

# Configure SDL by calling its CMake file.
# we use EXCLUDE_FROM_ALL so that its install targets and configs don't
# pollute upwards into our configuration.
add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)

# If you don't want SDL_ttf, then remove this section.
set(SDLTTF_VENDORED ON) # tell SDL_ttf to build its own dependencies
add_subdirectory(extern/SDL_ttf EXCLUDE_FROM_ALL)

add_subdirectory(extern/glm EXCLUDE_FROM_ALL)




#FetchContent_MakeAvailable(SDL3_ttf)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")


add_executable(${PROJECT_NAME} ${SOURCES}
        src/ParticleSystem.cpp
        src/AssetManager.cpp
        src/GameObject.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/extern/stb
        ${PROJECT_SOURCE_DIR}/include
)

# Link libraries in a portable way
# Prefer OpenGL::GL if found; glad brings the headers + function pointers regardless.
if(TARGET OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/extern/stb
        ${PROJECT_SOURCE_DIR}/include
)


target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3_ttf::SDL3_ttf
        SDL3::SDL3
        glm::glm
        glad
)

if (MINGW)
    message("Statically linking files for distr")
    # Pull libstdc++ and libgcc into the exe
    target_link_options(Asteroids PRIVATE -static-libstdc++ -static-libgcc)
    # Optional: if you want to also try static winpthread (can be finicky):
    # target_link_options(Asteroids PRIVATE -static -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE USE_GLAD=1)

file(COPY ${CMAKE_SOURCE_DIR}/resources
        DESTINATION ${CMAKE_BINARY_DIR})
# Result: ${CMAKE_BINARY_DIR}/resources/...

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION resources)



if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources)
endif()




