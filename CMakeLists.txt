cmake_minimum_required(VERSION 3.10)

project(anjin)

set(DEMOS
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demos/demo1.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demos/demo2.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demos/demo3.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/demos/demo4.c
)

set(SPACE_CATET
  ${CMAKE_CURRENT_LIST_DIR}/src/game/space_catet/level.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/space_catet/title.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/space_catet/lose.c
  ${CMAKE_CURRENT_LIST_DIR}/src/game/space_catet/win.c
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
  ${DEMOS}
  ${SPACE_CATET}

  ${CMAKE_CURRENT_LIST_DIR}/src/driver.c
  ${MODULE_CORE}
  ${MODULE_ECS}
  ${MODULE_GFX}
  ${MODULE_GL}
  ${MODULE_SND}
  ${MODULE_TXT}
  ${CMAKE_CURRENT_LIST_DIR}/src/log/log.c
  
  ${CMAKE_CURRENT_LIST_DIR}/lib/lodepng/lodepng.c
)

target_link_directories(anjin PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/lib
)


set_target_properties(anjin PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY bin
)

if (MSVC)
  set(CMAKE_C_FLAGS "/D _CRT_SECURE_NO_WARNINGS")
  set(LIBS "opengl32.lib openal.lib freetype.lib")
endif ()

if (EMSCRIPTEN)
  set_target_properties(anjin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY html
  )
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} \
    -s OFFSCREEN_FRAMEBUFFER=1 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    --preload-file res \
    -pthread \
    -s PTHREAD_POOL_SIZE=2 \
    --profiling --profiling-funcs \
    ")
    set(LIBS "-GL -lopenal -lfreetype -lpthread -lm")
endif ()


target_link_libraries(anjin PUBLIC
  ${LIBS}
)

target_include_directories(anjin PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/src
  ${CMAKE_CURRENT_LIST_DIR}/lib
)