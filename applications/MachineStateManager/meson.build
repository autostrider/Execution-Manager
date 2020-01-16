inc = include_directories('include')

msm_includes_dep = declare_dependency(
  include_directories : inc,
)

msm_source_files = [
  'machine_state_manager.cpp',
  'msm_main.cpp',
  'msm_state_machine.cpp',
  'socket_interface.cpp',
  'socket_server.cpp',
]

msm_source_files_with_path = []

foreach i : msm_source_files
  msm_source_files_with_path += './src/' + i
endforeach

executable(
  'msm', 
  sources : [ 
    msm_source_files_with_path, 
    capnproto_header_files
  ],
  include_directories : inc,
  dependencies : [
    app_state_client_dep,
    generated_dep,
    machine_state_client_dep,
    common_dep,
    json_dep,
    per_dep,
    capnp_rpc_dep,
  ]
)