#!python
import os
import sys

# Reads variables from an optional file.
customs = ['../custom.py']
opts = Variables(customs, ARGUMENTS)

# Gets the standart flags CC, CCX, etc.
env = Environment(ENV = os.environ)

# Define our parameters
opts.Add(EnumVariable('target', "Compilation target", 'release', ['d', 'debug', 'r', 'release']))
opts.Add(EnumVariable('platform', "Compilation platform", 'windows', ['windows', 'x11', 'linux', 'osx']))
opts.AddVariables(
    PathVariable('openvr_path', 'The path where the OpenVR repo is located.', 'openvr/'),
    PathVariable('target_path', 'The path where the lib is installed.', 'demo/addons/godot-openvr/bin/'),
    PathVariable('target_name', 'The library name.', 'libgodot_openvr', PathVariable.PathAccept),
)
opts.Add(BoolVariable('use_mingw', "Cross-compile for Windows", 'no'))
opts.Add(BoolVariable('use_llvm', "Use the LLVM / Clang compiler", 'no'))
opts.Add(EnumVariable('bits', "CPU architecture", '64', ['32', '64']))

# Other needed paths
godot_headers_path = "godot-cpp/godot-headers/"
godot_cpp_path = "godot-cpp/"
godot_cpp_library = "libgodot-cpp"

# Updates the environment with the option variables.
opts.Update(env)

# Check some environment settings
if env['use_llvm']:
    env['CXX'] = 'clang++'

# fix needed on OSX
def rpath_fix(target, source, env):
    os.system('install_name_tool -id @rpath/libgodot_openvr.dylib {0}'.format(target[0]))
    os.system('install_name_tool -change @rpath/OpenVR.framework/Versions/A/OpenVR @loader_path/OpenVR.framework/Versions/A/OpenVR {0}'.format(target[0]))

# platform dir for openvr libraries
platform_dir = ''

# openvr support dll
openvr_dll_source = '';
openvr_dll_target = '';

# Setup everything for our platform
if env['platform'] == 'windows':
    env['target_path'] += 'win' + env['bits'] + '/'
    godot_cpp_library += '.windows'
    platform_dir = 'win' + str(env['bits'])
    # Try to detect the host platform automatically.
    # This is used if no `platform` argument is passed
    if sys.platform.startswith('linux'):
        host_platform = 'linux'
    elif sys.platform == 'darwin':
        host_platform = 'osx'
    elif sys.platform == 'win32' or sys.platform == 'msys':
        host_platform = 'windows'
    else:
        raise ValueError(
            'Could not detect host_platform automatically'
        )

    if host_platform == 'windows'and not env["use_mingw"]:
        if not env['use_llvm']:
            # This makes sure to keep the session environment variables on windows,
            # that way you can run scons in a vs 2017 prompt and it will find all the required tools
            env.Append(ENV = os.environ)
        # MSVC
        env.Append(CCFLAGS = ['-DWIN32', '-D_WIN32', '-D_WINDOWS', '-W3', '-GR', '-D_CRT_SECURE_NO_WARNINGS', '/std:c++17'])
        env.Append(CPPDEFINES=["TYPED_METHOD_BIND"])
        env.Append(LINKFLAGS=["/WX"])
        if env['target'] in ('debug', 'd'):
            env.Append(CCFLAGS = ['-EHsc', '-D_DEBUG', '-MDd', '-Zi', '-FS'])
            env.Append(LINKFLAGS = ['-DEBUG:FULL'])
        else:
            env.Append(CCFLAGS = ['-O2', '-EHsc', '-DNDEBUG', '-MD', '-Zi', '-FS'])
            env.Append(LINKFLAGS = ['-DEBUG:FULL'])
    elif host_platform == "windows" and env["use_mingw"]:
        # Don't Clone the environment. Because otherwise, SCons will pick up msvc stuff.
        env = Environment(ENV=os.environ, tools=["mingw"])
        opts.Update(env)
        # env = env.Clone(tools=['mingw']) 
        env.Append(CCFLAGS = ['-std=c++17'])
        env.Append(LINKFLAGS=[ 
            '-static-libgcc',
            '-static-libstdc++',
        ])

    elif host_platform == 'linux' or host_platform == 'osx':
        # Cross-compilation using MinGW
        if env['bits'] == '64':
            mingw_prefix = 'x86_64-w64-mingw32-'
        elif env['bits'] == '32':
            mingw_prefix = 'i686-w64-mingw32-'
        if env["use_llvm"]:
            env["CC"] = mingw_prefix + "clang"
            env["AS"] = mingw_prefix + "as"
            env["CXX"] = mingw_prefix + "clang++"
            env["AR"] = mingw_prefix + "ar"
            env["RANLIB"] = mingw_prefix + "ranlib"
            env["LINK"] = mingw_prefix + "clang++"
            env.Append(LINKFLAGS=["-Wl,-pdb="])
            env.Append(CCFLAGS=["-gcodeview"])
        else:
            env["CC"] = mingw_prefix + "gcc"
            env["AS"] = mingw_prefix + "as"
            env["CXX"] = mingw_prefix + "g++"
            env["AR"] = mingw_prefix + "gcc-ar"
            env["RANLIB"] = mingw_prefix + "gcc-ranlib"
            env["LINK"] = mingw_prefix + "g++"
        env["SHCCFLAGS"] = '$CCFLAGS'

        # Native or cross-compilation using MinGW
        env.Append(CCFLAGS=['-g', '-O3', '-std=c++17', '-Wwrite-strings'])
        env.Append(LINKFLAGS=[
            '-static-libgcc',
            '-static-libstdc++',
        ])
        env['target_name'] += ".dll"

# no longer supported by OpenVR
#elif env['platform'] == 'osx':
#    env['target_path'] += 'osx/'
#    godot_cpp_library += '.osx'
#    platform_dir = 'osx32' # on OSX this is a universal binary
#    if env['target'] in ('debug', 'd'):
#        env.Append(CCFLAGS = ['-g','-O2', '-arch', 'x86_64'])
#    else:
#        env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64'])
#    env.Append(CXXFLAGS='-std=c++11')
#    env.Append(LINKFLAGS = ['-arch', 'x86_64'])
#
#    openvr_dll_target = env['target_path'] + "???"
#    openvr_dll_source = env['openvr_path'] + "bin/osx" + str(env['bits']) + "/???"

elif env['platform'] in ('x11', 'linux'):
    env['target_path'] += 'x11/'
    godot_cpp_library += '.linux'
    platform_dir = 'linux' + str(env['bits'])
    if env['target'] in ('debug', 'd'):
        env.Append(CCFLAGS = ['-fPIC', '-g3','-Og', '-std=c++17'])
    else:
        env.Append(CCFLAGS = ['-fPIC', '-g','-O3', '-std=c++17'])
    env.Append(CXXFLAGS='-std=c++0x')
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

    openvr_dll_target = env['target_path'] + "libopenvr_api.so"
    openvr_dll_source = env['openvr_path'] + "bin/linux" + str(env['bits']) + "/libopenvr_api.so"

# Complete godot-cpp library path
if env['target'] in ('debug', 'd'):
    godot_cpp_library += '.debug'
    env.Append(CPPDEFINES=["DEBUG_ENABLED", "DEBUG_METHODS_ENABLED"])
else:
    godot_cpp_library += '.release'

godot_cpp_library += '.' + str(env['bits'])

# Update our include search path
env.Append(CPPPATH=[
    '.',
    'src/',
    'src/open_vr/',
    godot_headers_path,
    godot_cpp_path + 'include/',
    godot_cpp_path + 'gen/include/',
    env['openvr_path'] + 'headers/'])

# Add our godot-cpp library
env.Append(LIBPATH=[godot_cpp_path + 'bin/'])
env.Append(LIBS=[godot_cpp_library])

if (os.name == "nt" and os.getenv("VCINSTALLDIR")) and not env["use_mingw"]:
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LINKFLAGS=['openvr_api.lib'])
elif env['platform'] == "osx" and not env["use_mingw"]:
    env.Append(LINKFLAGS = ['-F' + env['openvr_path'] + 'bin/osx64', '-framework', 'OpenVR'])
elif env['platform'] == "windows" and env['use_llvm']:
    env.Append(LINKFLAGS=[env['openvr_path'] + 'lib/' + platform_dir + '/openvr_api.lib'])
else:
    env.Append(LIBPATH=[env['openvr_path'] + 'lib/' + platform_dir])
    env.Append(LIBS=['openvr_api'])

# Add our sources
sources = Glob('src/*.c')
sources += Glob('src/*.cpp')
sources += Glob('src/*/*.c')
sources += Glob('src/*/*.cpp')

# Build our library
library = env.SharedLibrary(target=env['target_path'] + env['target_name'], source=sources)
if env['platform'] == "osx":
    env.AddPostAction(library, rpath_fix)

if openvr_dll_target != '':
    env.AddPostAction(library, Copy(
        openvr_dll_target,
        openvr_dll_source
    ))

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))
