project('05_uml_2_restino', 'cpp', 
        default_options: ['cpp_std=c++2a', 'warning_level=3', 'werror=true'])

add_global_arguments('-lssl', language: 'cpp')
add_global_arguments('-lcrypto', language: 'cpp')
add_global_arguments('-Wno-unknown-pragmas', language: 'cpp')

inc_dir = include_directories(['include',
                               get_option('cli11_include_dir'),
                               get_option('fmt_include_dir'),
                               get_option('spdlog_include_dir'),
                               get_option('asio_include_dir'),
                               '../libs/cpr/include/',
                               '../libs/http-parser/',
                               '../libs/restinio/'])

cc = meson.get_compiler('cpp')
lib_fmt = cc.find_library('fmt', dirs: [meson.current_source_dir() + '/../fmt-6.1.2/build/'])
lib_http_parser = cc.find_library('http_parser', dirs: [meson.current_source_dir() + '/libs/http-parser/build/'])
lib_curl = cc.find_library('curl', dirs: [meson.current_source_dir() + '/libs/cpr/build/lib'])
lib_cpr = cc.find_library('cpr', dirs: [meson.current_source_dir() + '/libs/cpr/build/lib'])

srcClient = ['src/client/main.cpp', 'src/base64.cpp']
srcServer = ['src/server/main.cpp', 'src/base64.cpp', 'src/pystring.cpp'] 

ssl_dep = dependency('openssl')
thread_dep = dependency('threads')

executable('client',
            sources: srcClient,
            include_directories: inc_dir,
            dependencies: [lib_curl, lib_cpr])

executable('server',
            sources: srcServer,
            include_directories: inc_dir,
            dependencies: [lib_fmt, lib_http_parser, thread_dep, ssl_dep])