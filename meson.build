project('winter', 'cpp', default_options: ['cpp_std=c++23'])

cpp_flags = ['-Wall', '-Wextra', '-Wconversion', '-Wimplicit-fallthrough', '-g']
llvm_dep = dependency('llvm', version: '>=22.0', modules: ['core'])

CXX = meson.get_compiler('cpp')
lldelf_dep = CXX.find_library('liblldELF', dirs: ['/usr/lib64'])
lldcommon_dep = CXX.find_library('liblldCommon', dirs: ['/usr/lib64'])

src_files = [
    'src/frontend/lexer.cpp',
    'src/frontend/parser.cpp',
    'src/backend/backend.cpp',
]
winter_src = static_library(
    'winter_src',
    src_files,
    cpp_args: cpp_flags,
    dependencies: [llvm_dep, lldelf_dep, lldcommon_dep],
    include_directories: '/usr/include/lld/Common',
)

executable('winter', 'src/main.cpp', cpp_args: cpp_flags, link_with: winter_src)

# tests
willow = dependency('willow', method: 'cmake')
executable(
    'test_exe',
    'tests/test.cpp',
    link_with: winter_src,
    cpp_args: cpp_flags,
    dependencies: willow,
    include_directories: 'src',
)
