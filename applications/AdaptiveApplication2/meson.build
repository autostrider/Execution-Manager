executable(
  'proc2',
  sources : [ 
    './src/proc2.cpp',
    aa1_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : adaptive_app_inc, 
  dependencies : [
    app_state_client_dep,
    generated_dep,
    component_client_dep,
    common_dep,
    kj_async_dep,
    capnp_rpc_dep
  ]
)

executable(
  'proc3',
  sources : [ 
    './src/proc3.cpp',
    aa1_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : adaptive_app_inc, 
  dependencies : [
    app_state_client_dep,
    generated_dep,
    component_client_dep,
    common_dep,
    kj_async_dep,
    capnp_rpc_dep
  ]
)