project('05_uml_2_restino', 'cpp', 
        default_options: ['cpp_std=c++2a', 'warning_level=3', 'werror=true'])

inc_dir = include_directories([get_option('include_dir'),
                               get_option('cli11_include_dir'),
                               get_option('spdlog_include_dir'),
                               get_option('cpr_include_dir')])

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
            dependencies: thread_dep)