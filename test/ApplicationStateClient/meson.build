as_test_source_files = [
  'application_state_client_test.cpp'
]

as_test = executable(
  'as_test', 
  sources : [ 
    as_test_source_files,
    capnproto_header_files
  ],
  dependencies : [
    app_state_client_dep,
    common_dep,
    generated_dep,
    gtest_dep,
    gmock_dep,
    capnp_rpc_dep
  ]
)

test('ApplicationStateClient', as_test)