project('mpv-controller', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

libsocks_proj = subproject('socks')
libsocks_dep = libsocks_proj.get_variable('libsocks_dep')

executable('mpv-controller',
           'main.cpp',
           'Interface_socks.cpp',
           dependencies : [ dependency('threads'), libsocks_dep ],
           install : false)
