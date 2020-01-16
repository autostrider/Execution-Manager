inc = include_directories('include')

msmc_source_files = [
  'machine_state_client.cpp',
  'i_machine_state_client_wrapper.cpp'
]

msmc_source_files_with_path = []

foreach i : msmc_source_files
  msmc_source_files_with_path += './src/' + i
endforeach

machine_state_client_lib = static_library(
  'machine_state_client_lib', 
  sources : [
    msmc_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : [
    inc,
  ],
  dependencies : [
    common_dep,
    generated_dep,
    kj_dep,
  ]
)

machine_state_client_dep = declare_dependency(
  link_with : machine_state_client_lib,
  include_directories : inc
)