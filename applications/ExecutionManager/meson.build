inc = include_directories('include')

em_headers_dep = declare_dependency(
  include_directories : inc
)

em_source_files = [
  'application_handler.cpp',
  'execution_manager_client.cpp',
  'execution_manager.cpp',
  'execution_manager_server.cpp',
  'msm_handler.cpp',
  'os_interface.cpp'
]

em_source_files_with_path = []

foreach i : em_source_files
  em_source_files_with_path += './src/' + i
endforeach

em_objects = static_library(
  'em', 
  sources : [
    em_source_files_with_path, 
    capnproto_header_files
  ],
  include_directories : inc,
  dependencies : [
    common_dep,
    json_dep,
    generated_dep,
    capnp_rpc_dep,
    kj_async_dep
  ]
)

em_dep = declare_dependency(
  link_with : em_objects,
  include_directories : inc,
)

executable(
  'em', 
  sources : [
    './src/em_main.cpp',
    capnproto_header_files
  ],
  dependencies : [
    em_dep,
    common_dep,
    json_dep
  ]
)