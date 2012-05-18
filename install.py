#!/usr/bin/python

import sys
import json
import os
import shlex
import subprocess
import shutil
import tempfile
import re

def main(pname, args):

	# Read arguments
	user_parts = [ 'main' ]
	user_command = ''
	arg_id = 0
	while arg_id < len(args):
		arg = args[arg_id]
		arg_id += 1
		if arg == '-p' or arg == '--parts':
			if arg_id == len(args):
				raise Exception('Parts missing!')
			parts_str = args[arg_id]
			arg_id += 1
			user_parts = parts_str.split(',')
		elif len(user_command) == 0:
			user_command = arg
		else:
			raise Exception('Invalid parameter \"' + arg + '\"!')

	# Fix user arguments
	if len(user_command) == 0:
		user_command = 'build'

	# Read install data
	data_file = open('install.json', 'r')
	data = json.load(data_file)
	data_file.close()

	# Form final parts
	parts = set()
	for part_name in user_parts:
		if part_name not in data['parts']:
			raise Exception('There is no part \"' + part_name + '\"!')
		part = data['parts'][part_name]
		parts.add(part_name)
		if 'deps' in part:
			for dep in part['deps']:
				parts.add(dep)

	# Combine headers and sources
	headers = []
	sources = []
	for part_name in parts:
		part = data['parts'][part_name]
		if 'headers' in part:
			headers += part['headers']
		if 'sources' in part:
			sources += part['sources']

	# Form compiler flags and libs
	cflags = '-O3 -DNDEBUG'
	libs = ''
	for part_name in parts:
		part = data['parts'][part_name]
		if 'cflags' in part:
			cflags += ' ' + part['cflags']
		if 'libs' in part:
			libs += ' ' + part['libs']

	# Some options
	desc = data['short_description']

	# Some hard coded options
	libname = 'hpp'
	arch = 'amd64'
	version = '0.' + runCommand('svn info 2> /dev/null|grep Revision|cut -d \" \" -f 2', return_output = True).split('\n')[0]
	install_path = '/usr'

	# Run commands
	if user_command == 'build':
		build(sources, cflags, libs, libname)
	elif user_command == 'test':
		test()
	elif user_command == 'install':
		build(sources, cflags, libs, libname)
		install(install_path, headers, libname, libs, version, desc)
		clean(sources, libname)
	elif user_command == 'uninstall':
		uninstall(install_path, headers, libname)
	elif user_command == 'deb':
		# Test before doing anything else		
		test()

		# Prepare
		temp = tempfile.mkdtemp()
		temp_install = os.path.join(temp, 'data', 'usr')
		wd = os.getcwd()

		# Get Debian dependencies
		deb_deps_set = set()
		for part_name in user_parts:
			if part_name not in data['parts']:
				raise Exception('There is no part \"' + part_name + '\"!')
			part = data['parts'][part_name]
			parts.add(part_name)
			if 'deb_deps' in part:
				for dep in part['deb_deps']:
					deb_deps_set.add(dep)
		deb_deps_list = list(deb_deps_set)
		deb_deps = ''
		for deb_dep_id in range(len(deb_deps_list)):
			deb_dep = deb_deps_list[deb_dep_id]
			deb_deps += deb_dep
			if deb_dep_id < len(deb_deps_list) - 1:
				deb_deps += ', '

		# Build and install to temporary
		build(sources, cflags, libs, libname)
		install(temp_install, headers, libname, libs, version, desc, real_path = '/usr')

		# Pack and calculate data
		os.chdir(os.path.join(temp, 'data'))
		runCommand('tar czf ../data.tar.gz *')
		data_size = int(runCommand('du -s|cut -f 1', return_output = True))

		# Create control files
		ensureDirExists(os.path.join(temp, 'control'))
		os.chdir(os.path.join(temp, 'data'))
		runCommand('find *|xargs md5sum 2> /dev/null > ../control/md5sums || true')
		control_file = open(os.path.join(temp, 'control', 'control'), 'w')
		control_file.write('Package: lib' + libname + '-dev\n')
		control_file.write('Version: ' + version + '\n')
		control_file.write('Architecture: ' + arch + '\n')
		control_file.write('Maintainer: ' + data['deb_maintainer_name'] + ' <' + data['deb_maintainer_email'] + '>\n')
		control_file.write('Installed-Size: ' + str(data_size) + '\n')
		control_file.write('Depends: ' + deb_deps + '\n')
		control_file.write('Section: libdevel\n')
		control_file.write('Priority: optional\n')
		control_file.write('Homepage: http://www.henu.fi/\n')
		control_file.write('Description: LibHPP\n')
		control_file.write(' ' + desc + '\n')
		control_file.close()
		os.chdir(os.path.join(temp, 'control'))
		runCommand('tar czf ../control.tar.gz *')
		
		# Create other package files
		control_file = open(os.path.join(temp, 'debian-binary'), 'w')
		control_file.write('2.0\n')
		control_file.close()

		# Make package
		os.chdir(temp)
		deb_file = 'lib' + libname + '-dev_' + version + '_' + arch + '.deb'
		runCommand('ar r ' + addSlashes(os.path.join(wd, deb_file)) + ' debian-binary control.tar.gz data.tar.gz 2> /dev/null')

		# Return to original working directory
		os.chdir(wd)

		# Clean
		clean(sources, libname)
	elif user_command == 'freebsd':
		cflags += ' -I/usr/local/include/ -DFREEBSD'
		if '3d' not in parts and 'image' not in parts:
			cflags += ' -DHPP_NO_SDL'
		build(sources, cflags, libs, libname)
	else:
		raise Exception('Invalid command \"' + user_command + '\"!')

	return 0

def build(sources, cflags, libs, libname):
	# Compile
	objs_str = ''
	objs = []
	for source in sources:
		source_prefix = source.split('.')[0]
		obj = source_prefix + '.o'
		objs_str += addSlashes(obj) + ' '
		objs.append(obj)
		runCommand('g++ -fPIC -w -c ' + cflags + ' ' + addSlashes(source) + ' -o ' + obj)
	# Link
	runCommand('g++ -w -shared ' + cflags + ' -o lib' + addSlashes(libname) + '.so ' + objs_str)

def clean(sources, libname):
	# Clean object files
	for source in sources:
		source_prefix = source.split('.')[0]
		obj = source_prefix + '.o'
		os.remove(obj)
	# Clean library
	os.remove('lib' + libname + '.so')

def test():
	runCommand('g++ -Wall -Wpointer-arith -Werror -ansi -pedantic -o /tmp/libhpp_tester test.cc 3dconstants.cc assert.cc -lcrypto')
	runCommand('/tmp/libhpp_tester')
	os.remove('/tmp/libhpp_tester')

def install(path, headers, libname, libs, version, desc, real_path = None):

	# TODO: Get these from somewhere else!
	deps = ''

	if real_path == None:
		real_path = path

	# Dirs
	headers_dir = os.path.join(path, 'include', 'hpp')
	lib_dir = os.path.join(path, 'lib')
	pc_dir = os.path.join(path, 'lib', 'pkgconfig')

	# Install headers
	ensureDirExists(headers_dir)
	for header in headers:
		header_subdirs = os.path.dirname(header)
		ensureDirExists(os.path.join(headers_dir, header_subdirs))
		shutil.copyfile(header, os.path.join(headers_dir, header))
	# Install lib
	ensureDirExists(lib_dir)
	shutil.copyfile('lib' + libname + '.so', os.path.join(lib_dir, 'lib' + libname + '.so'))

	# Write .pc file
	ensureDirExists(pc_dir)
	pc_file = open(os.path.join(pc_dir, libname + '.pc'), 'w')
	pc_file.write('prefix=' + real_path + '\n')
	pc_file.write('exec_prefix=${prefix}\n')
	pc_file.write('libdir=${exec_prefix}/lib\n')
	pc_file.write('includedir=${prefix}/include/hpp\n')
	pc_file.write('\n')
	pc_file.write('Name: ' + libname + '\n')
	pc_file.write('Description: ' + desc + '\n')
	pc_file.write('Version: ' + version + '\n')
	pc_file.write('Requires: ' + deps + '\n')
	pc_file.write('Libs: -L${libdir} -lhpp ' + libs + '\n')
	pc_file.write('Cflags: \n')
	pc_file.close()

def uninstall(path, headers, libname):

	# Dirs
	headers_dir = os.path.join(path, 'include', 'hpp')
	lib_dir = os.path.join(path, 'lib')
	pc_dir = os.path.join(path, 'lib', 'pkgconfig')

	# Remove headers
	removeAsMuchAsPossible(headers_dir)
	for header in headers:
		header_subdirs = os.path.dirname(header)
		ensureDirExists(os.path.join(headers_dir, header_subdirs))
		removeAsMuchAsPossible(os.path.join(headers_dir, header))

	# Remove lib
	removeAsMuchAsPossible(lib_dir)
	removeAsMuchAsPossible(os.path.join(lib_dir, 'lib' + libname + '.so'))

	# Remove .pc file
	removeAsMuchAsPossible(pc_dir)
	removeAsMuchAsPossible(os.path.join(pc_dir, libname + '.pc'))

def runCommand(command, return_output = False):
	print command
	if return_output:
		p = subprocess.Popen(command, stdout = subprocess.PIPE, stderr = subprocess.PIPE, shell = True)
		output = p.communicate()
		if p.returncode:
		        raise Exception(output[1])
		return output[0]
	else:
		p = subprocess.Popen(command, shell = True)
		p.wait()
	        if p.returncode:
	                raise Exception('Running of command \"' + command + '\" failed!')
	        return

def ensureDirExists(path, mode = 0755):
	if os.path.exists(path):
		return
	os.makedirs(path, mode)

def removeAsMuchAsPossible(path):
	if os.path.isfile(path):
		os.remove(path)
		try:
			os.removedirs(os.path.dirname(path))
		except:
			pass
	else:
		try:
			os.removedirs(path)
		except:
			pass

def addSlashes(s):
	s = s.replace(' ', '\\ ')
	s = s.replace('\"', '\\\"')
	s = s.replace('\'', '\\\'')
	return s

if __name__ == '__main__':
	try:
		retval = main(sys.argv[0], sys.argv[1:])
	except StandardError:
		raise
	except Exception as e:
		print 'ERROR: ' + str(e)
		sys.exit(1)
	sys.exit(retval)

