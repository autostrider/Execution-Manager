capnp = find_program('capnp')

schema_files = [
  'application_state_management.capnp',
  'component_state_management.capnp',
  'execution_management.capnp',
  'machine_state_management.capnp'
]

output_source_files = []
output_header_files = []

foreach i : schema_files
  output_header_files += i + '.h'
  output_source_files += i + '.c++'
endforeach

capnproto_source_files = custom_target(
  'capnproto_source_files',
  command: [
    capnp, 
    'compile',
    '--src-prefix=@SOURCE_DIR@/gen/',
    '-oc++:@OUTDIR@',
    '@INPUT@',
  ],
  input : schema_files,
  output : output_source_files
)

capnproto_header_files = custom_target(
  'capnproto_header_files',
  command: [
    capnp, 
    'compile',
    '--src-prefix=@SOURCE_DIR@/gen/',
    '-oc++:@OUTDIR@',
    '@INPUT@',
  ],
  input : schema_files,
  output : output_header_files
)

generated_lib = static_library(
  'generated_lib',
  sources : [ 
    capnproto_header_files, 
    capnproto_source_files
  ]
)

generated_dep = declare_dependency(
  link_with : generated_lib,
)
