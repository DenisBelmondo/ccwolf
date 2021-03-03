#!/usr/bin/env python3


#
# names of libraries to link to
#

libs = ['SDL2', 'SDL2main', 'SDL2_mixer']

sources = [
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
    'wl_text.cpp',
]


#
# environment stuff
#

env = Environment()

gpl                = ARGUMENTS.get('gpl', '')
target             = ARGUMENTS.get('target', 'debug')
sdl2_main_lib      = ARGUMENTS.get('sdl2_main_lib', '')
sdl2_include       = ARGUMENTS.get('sdl2_include', '')
sdl2_lib           = ARGUMENTS.get('sdl2_lib', '')
sdl2_mixer_include = ARGUMENTS.get('sdl2_mixer_include', '')
sdl2_mixer_lib     = ARGUMENTS.get('sdl2_mixer_lib', '')

if gpl:
    sources.insert(0, 'dosbox/fmopl.cpp')
else:
    sources.insert(0, 'mame/fmopl.cpp')

if env['PLATFORM'] == 'win32':
    libs.insert(0, 'shell32')

    env.Append(CCFLAGS=['-W3', '-GR'])
    if target in ('debug', 'd'):
        env.Append(CPPDEFINES=['_DEBUG'])
        env.Append(CCFLAGS=['-EHsc', '-MDd', '-ZI'])
        env['LINKFLAGS'] += ['-DEBUG']
    else:
        env.Append(CPPDEFINES=['NDEBUG'])
        env.Append(CCFLAGS=['-O2', '-EHsc', '-MD'])

    env['CPPDEFINES'] += ['SDL_MAIN_HANDLED']

elif env['PLATFORM'] == "osx":
    env.Append(CCFLAGS=['-arch', 'x86_64'])
    env.Append(CXXFLAGS=['-std=c++17'])
    env.Append(LINKFLAGS=['-arch', 'x86_64'])
    if target in ('debug', 'd'):
        env.Append(CCFLAGS=['-g', '-O2'])
    else:
        env.Append(CCFLAGS=['-g', '-O3'])

elif env['PLATFORM'] in ('x11', 'linux'):
    env.Append(CCFLAGS=['-fPIC'])
    env.Append(CXXFLAGS=['-std=c++17'])
    if target in ('debug', 'd'):
        env.Append(CCFLAGS=['-g3', '-Og'])
    else:
        env.Append(CCFLAGS=['-g', '-O3'])


#
# Do the program
#

env.Program(
    # program name
    'ccwolf',

    # source files
    sources,

    CPPPATH=[
        sdl2_include,
        sdl2_mixer_include
    ],

    LIBPATH=[
        sdl2_main_lib,
        sdl2_lib,
        sdl2_mixer_lib
    ],

    LIBS=libs
)
