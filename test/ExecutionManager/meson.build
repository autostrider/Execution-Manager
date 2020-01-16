em_test_source_files = [
  'application_handler_test.cpp',
  'execution_manager_test.cpp',
  'manifest_reader_test.cpp',
  'msm_register_test.cpp'
]

em_test = executable(
  'em_test', 
  sources : [ 
    em_test_source_files,
    capnproto_header_files
  ],
  dependencies : [
    em_dep,
    json_dep,
    app_state_client_dep,
    common_dep,
    gtest_dep,
    gmock_dep,
    mocks_dep
  ]
)

test(
  'ExecutionManager',
  em_test,
  timeout : 60
)