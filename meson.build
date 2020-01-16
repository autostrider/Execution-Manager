project(
  'Execution Manager', 
  'cpp',
  default_options : [
    'warning_level=3', 
    'cpp_std=c++14'])

add_global_arguments('-Wno-pedantic', language : 'cpp')

gen_output = include_directories('gen/')

common_headers = include_directories('common/include')
app_state_client_headers = include_directories('api/application_state_client/include')
component_client_headers = include_directories('api/component_client/include')

# Setting up JSON
json_proj = subproject('json')
json_dep = json_proj.get_variable('nlohmann_json_dep')

# Setting up Cap'n'Proto
capnp_dep = dependency('capnp')
kj_dep = dependency('kj')

capnp_rpc_dep = dependency('capnp-rpc')
kj_async_dep = dependency('kj-async')

# Setting up Google test
gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_main_dep')
gmock_dep = gtest_proj.get_variable('gmock_dep')

'''
cc = meson.get_compiler('cpp')
lib_hello = cc.find_library('libcapnp-0.6.1',
               dirs : ['/usr/lib/x86_64-linux-gnu'])
inc_hello = include_directories('/usr/lib/x86_64-linux-gnu')
'''

'''
cm = import('cmake')
subp = cm.subproject('capnproto', cmake_options : ['-DBUILD_TESTING=OFF'])

deps = [
  subp.dependency('capnp'),
  subp.dependency('kj'),
]

executable('cpn', dependencies: [deps])
'''

'''
# Use the code generator
generated = custom_target(
  'cmake-generated',
  input: [],
  output: ['test.cpp'],
  command: [capnp_target, '@OUTPUT@']
)
'''

#json_dep = json_proj.get_variable('nlohmann_json_dep')

subdir('gen')
subdir('common')
subdir('api')
subdir('per')
subdir('applications')
subdir('test')

#subdir('tools/json')
#subdir('tools/googletest')
#subdir('tools/capnproto')
