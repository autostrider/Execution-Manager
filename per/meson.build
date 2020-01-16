inc = include_directories('include')

per_source_files = [
  'keyvaluestorage.cpp',
  'kvstype.cpp'
]

per_source_files_with_path = []

foreach i : per_source_files
  per_source_files_with_path += './src/' + i
endforeach

per_lib = static_library(
  'per_lib', 
  sources : per_source_files_with_path,
  include_directories : [
    inc,
  ],
  dependencies : [
    json_dep,
    common_dep,
  ]
)

per_dep = declare_dependency(
  link_with : per_lib,
  include_directories : inc
)