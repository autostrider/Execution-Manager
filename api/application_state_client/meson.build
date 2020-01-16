inc = include_directories('include')

as_source_files = [
  'application_state_client.cpp',
  'i_application_state_client_wrapper.cpp'
]

as_source_files_with_path = []

foreach i : as_source_files
  as_source_files_with_path += './src/' + i
endforeach

app_state_client_lib = static_library(
  'app_state_client_lib', 
  sources : [ 
    as_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : [
    inc,
  ],
  dependencies : [
    common_dep,
    generated_dep
  ]
)

app_state_client_dep = declare_dependency(
  link_with : app_state_client_lib,
  include_directories : inc
)