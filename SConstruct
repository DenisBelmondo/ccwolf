#!/usr/bin/env python3


include_prefix = 'C:/Users/Mason/.opt/vcpkg/installed/x64-windows/include'
lib_prefix = 'C:/Users/Mason/.opt/vcpkg/installed/x64-windows/lib'
libs = ['SDL2', 'SDL2main', 'SDL2_mixer']

env = Environment()

if env['PLATFORM'] == 'win32':
    env['LINKFLAGS'] += ['/SUBSYSTEM:CONSOLE', '/NODEFAULTLIB:msvcrt']
    env.Append(CPPDEFINES=['WINDOWS_IGNORE_PACKING_MISMATCH'])
    libs.insert(0, 'shell32')

env.Program(
    # program name
    'wolf4sdl2',

    # source files
    [
        'mame/fmopl.cpp',

        'id_ca.cpp',
        'id_in.cpp',
        'id_pm.cpp',
        'id_sd.cpp',
        'id_us_1.cpp',
        'id_vh.cpp',
        'id_vl.cpp',
        'signon.cpp',
        'wl_act1.cpp',
        'wl_act2.cpp',
        'wl_agent.cpp',
        'wl_atmos.cpp',
        'wl_cloudsky.cpp',
        'wl_debug.cpp',
        'wl_draw.cpp',
        'wl_floorceiling.cpp',
        'wl_game.cpp',
        'wl_inter.cpp',
        'wl_main.cpp',
        'wl_menu.cpp',
        'wl_parallax.cpp',
        'wl_play.cpp',
        'wl_state.cpp',
        'wl_text.cpp'
    ],

    CPPPATH=[
        include_prefix + '/SDL2'
    ],

    LIBPATH=[
        lib_prefix,
        lib_prefix + '/manual-link'
    ],

    LIBS=libs
)
