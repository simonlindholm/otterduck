#!/usr/bin/env python

import Options
import os
VERSION = '0.1'
APPNAME = 'otterduck'

top = '.'
out = 'build'

def is_mingw (env):
	if 'CC' in env:
		cc = env['CC']
		if not isinstance (cc, str):
			cc = ''.join (cc)
		return cc.find ('mingw') != -1
	return False

def options(opt):
	opt.tool_options('compiler_cxx')

def configure(conf):
	conf.check_tool('compiler_cxx')

	# The below is some MinGW-specific configuration NieXS added. I'm not
	# sure what it does or why it is needed, but it looks important enough.
	conf.env['LMINGW'] = ''
	if is_mingw(conf.env):
		if not 'AR' in os.environ and not 'RANLIB' in os.environ:
			conf.env['AR'] = os.environ['CC'][:-3] + 'ar'
		Options.platform = 'win32'
		conf.env['program_PATTERN'] = '%s.exe'
		conf.env.append_value('CCFLAGS', '-mms-bitfields')
		conf.env.append_value ('CCFLAGS', '-mwindows')
		conf.env.append_value('CCFLAGS', '-DWINDOWS')
		conf.env["LMINGW"] = 'mingw32'

	conf.define('VERSION', VERSION)
	conf.env['VERSION'] = VERSION
	conf.write_config_header('config.h')
	conf.env.append_unique('CXXFLAGS', ['-O2', '-std=c++0x', '-Wall', '-Wextra', '-Wno-unused-parameter'])
	conf.env.append_unique('LDFLAGS', ['-O2', '-std=c++0x', '-Wall', '-Wextra'])

def build(bld):
	bld(features = 'cxx cxxprogram',
		source = bld.path.ant_glob('src/client/**/*.cpp'),
		lib = ['sfml-graphics', 'sfml-window', 'sfml-system', 'sfml-audio', 'Xrandr'],
		target = APPNAME)
