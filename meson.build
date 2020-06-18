#meson build file, see https://mesonbuild.com/Syntax.html

project('icoPack', 'cpp', default_options : [
	  'cpp_std=c++17'
	, 'layout=flat'
	, 'strip=false'
	, 'unity=off'
	, 'warning_level=3'])

add_global_arguments(
	  '-Wno-non-virtual-dtor', '-Wno-unused-parameter', '-Wno-unused-function', '-Wno-missing-field-initializers'
	, '-Wno-deprecated-copy', '-Wno-dangling-else', '-Wno-unused-result', '-Wno-class-memaccess', '-Wno-vla'

	, '-Wextra', '-Wall', '-Wformat', '-Wnull-dereference', '-Winit-self', '-Wstrict-overflow=5', '-Wstringop-overflow=4', '-Wstringop-truncation'
	, '-Wsuggest-attribute=const', '-Wsuggest-attribute=malloc', '-Wsuggest-attribute=format', '-Wmissing-format-attribute', '-Wsuggest-attribute=cold'
	, '-Wattribute-alias=2', '-Wduplicated-branches', '-Wtrampolines', '-Wunsafe-loop-optimizations'
	, '-Wplacement-new=2', '-Wpointer-arith', '-Wcast-align', '-Wempty-body', '-Wenum-compare', '-Wlogical-op'
	, '-Wredundant-decls', '-Winvalid-pch', '-Wdisabled-optimization'

	, '-fmax-errors=10'
	, '-DDEBUG', '-g'
#	, '-Os'
	, language: 'cpp')

executable('icoPack'
, sources: ['main.cpp']
, include_directories: []
, cpp_pch: '.pch/pch.h'
#, link_with: 
#, link_args: []
)