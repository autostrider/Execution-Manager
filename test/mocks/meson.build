
inc = include_directories('include')

mock_source_files = [
  'i_machine_state_manager_mock.cpp',
]

mock_source_files_with_path = []

foreach i : mock_source_files
  mock_source_files_with_path += './src/' + i
endforeach


mocks_lib = static_library(
  'mocks',
  sources : [
    mock_source_files_with_path,
    capnproto_header_files
  ],
  include_directories : [
    inc,
  ],
  dependencies : [
    msm_includes_dep,
    app_state_client_dep,
    machine_state_client_dep,
    gmock_dep,
    gtest_dep,
    common_dep,
    json_dep,
    per_dep
  ]
)

mocks_dep = declare_dependency(
  link_with : mocks_lib,
  include_directories : inc,
)