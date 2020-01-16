adaptive_app_inc = include_directories('include')

aa1_source_files = [
  'adaptive_app.cpp',
  'mean_calculator.cpp',
  'state.cpp',
]

aa1_source_files_with_path = []

foreach i : aa1_source_files
  aa1_source_files_with_path += meson.current_source_dir() + '/src/' + i
endforeach

executable(
  'proc1',
  sources : [
    './src/aa_main.cpp', 
    aa1_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : [
    adaptive_app_inc,
  ],
  dependencies : [
    app_state_client_dep,
    generated_dep,
    component_client_dep,
    common_dep,
    kj_async_dep,
    capnp_rpc_dep
  ]
)