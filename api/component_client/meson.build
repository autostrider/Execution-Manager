inc = include_directories('include')

component_client_source_files = [
  'component_client.cpp',
  'i_component_client_wrapper.cpp'
]

component_client_source_files_with_path = []

foreach i : component_client_source_files
  component_client_source_files_with_path += './src/' + i
endforeach

component_client_lib = static_library(
  'component_client_lib', 
  sources : [ 
    component_client_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : [
    inc,
  ],
  dependencies : [
    common_dep,
    generated_dep,
  ]
)

component_client_dep = declare_dependency(
  link_with : component_client_lib,
  include_directories : inc
)