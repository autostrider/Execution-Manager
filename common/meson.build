inc = include_directories('include')

common_source_files = [
  'common.cpp',
  'manifest_reader.cpp',
  'string_safe_queue.cpp',
  'logger.cpp',
  'manifests.cpp',
]

common_source_files_with_path = []

foreach i : common_source_files
  common_source_files_with_path += './src/' + i
endforeach

common_lib = static_library(
  'common',
  sources : common_source_files_with_path,
  include_directories : [
    inc,
  ],
  dependencies : [
    json_dep
  ]
)

common_dep = declare_dependency(
  link_with : common_lib,
  include_directories : inc,
)