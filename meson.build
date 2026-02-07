project('winter', 'cpp', default_options: ['cpp_std=c++23'])
cpp_flags = ['-Wall', '-Wextra', '-Wconversion', '-Wimplicit-fallthrough']

src_files = [
  'src/compiler.cpp',
    'src/lexer.cpp'
]
winter_src = static_library('winter_src', src_files, cpp_args: cpp_flags)
executable('winter', 'src/main.cpp', cpp_args: cpp_flags, link_with: winter_src)
