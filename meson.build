project('mpv-controller', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=none'])
# Fix for old meson where cpp_std=c++20 is not available
add_project_arguments('-std=c++20', language : 'cpp')

executable('mpv-controller',
           'main.cpp',
           dependencies : dependency('threads'),
           install : false)
