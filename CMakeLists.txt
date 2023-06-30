cmake_minimum_required(VERSION 3.10)

project(anjin)

set(GAME
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demo1.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demo2.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demo3.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demo4.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/level.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/title.c
)

set(MODULE_CORE
  ${CMAKE_CURRENT_LIST_DIR}/src/core/core.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/input/input.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/thread/thread.c
  ${CMAKE_CURRENT_LIST_DIR}/src/core/window/window.c
)

set(MODULE_ECS
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/ecs.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/type.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/c/c.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/c/cl.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/e/e.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/e/em.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/q/q.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/t/t.c
  ${CMAKE_CURRENT_LIST_DIR}/src/ecs/t/tm.c
)

set(MODULE_GFX
  ${CMAKE_CURRENT_LIST_DIR}/src/gfx/gfx.c
)

set(MODULE_GL
  ${CMAKE_CURRENT_LIST_DIR}/src/gl/gl_util.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gl/gll.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gl/shader.c
  ${CMAKE_CURRENT_LIST_DIR}/src/gl/texture.c
)

set(MODULE_SND
  ${CMAKE_CURRENT_LIST_DIR}/src/snd/snd.c
  ${CMAKE_CURRENT_LIST_DIR}/src/snd/wav.c
)

set(MODULE_TXT
  ${CMAKE_CURRENT_LIST_DIR}/src/txt/txt.c
)

add_executable(anjin
  ${CMAKE_CURRENT_LIST_DIR}/src/driver.c
  ${MODULE_CORE}
  ${MODULE_ECS}
  ${MODULE_GFX}
  ${MODULE_GL}
  ${MODULE_SND}
  ${MODULE_TXT}
  ${CMAKE_CURRENT_LIST_DIR}/src/log/log.c
  
  ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.c

  ${GAME}
)

target_link_directories(anjin PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_include_directories(anjin PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_link_libraries(anjin PUBLIC
  opengl32 openal freetype
)

if (MSVC)
  set(CMAKE_C_FLAGS "/D _CRT_SECURE_NO_WARNINGS")
endif ()

set_target_properties(anjin PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY bin
)