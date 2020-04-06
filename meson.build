project('05_uml_2_restino', 'cpp', 
        default_options: ['cpp_std=c++2a', 'warning_level=3', 'werror=true'])

inc_dir = include_directories([get_option('include_dir'),
                               get_option('cli11_include_dir'),
                               get_option('spdlog_include_dir'),
                               get_option('cpr_include_dir'),
                               get_option('asio_include_dir'),
                               get_option('fmt_include_dir'),
                               get_option('http_parser_dir'),
                               get_option('restinio_include_dir')])

cc = meson.get_compiler('cpp')
lib_fmt = cc.find_library('fmt', dirs: ['/home/bernhard/Desktop/vorgabe/software/fmt-6.1.2/build/'])
lib_http_parser = cc.find_library('http_parser', dirs: ['/home/bernhard/Desktop/http-parser/build/'])

srcClient = ['src/client/main.cpp']
srcServer = ['src/server/main.cpp'] 

thread_dep = dependency('threads')

executable('client',
            sources: srcClient,
            include_directories: inc_dir,
            dependencies: thread_dep)

executable('server',
            sources: srcServer,
            include_directories: inc_dir,
            dependencies: [lib_fmt, lib_http_parser, thread_dep])