#!BPY
# coding=UTF-8
"""
Name: 'Hpp Importer'
Blender: 249
Group: 'Import'
Tooltip: 'Hpp Mesh and Scene Importer'
"""
import hpp_misc
import hpp_types
import Blender
import bpy
import struct
import copy
import math
from Blender import Draw, Window, Mathutils
from Blender.Mathutils import Vector
from array import array

class Importer:

	def __init__(self):
		self.meshes = []

	def read(self, filename):
		ifile = open(filename, 'rb')
		# Read header
		header = ifile.read(32)
		try:
			if header == 'HPP_MESH_1.01                   ':
				self.readMesh100(ifile)
			elif header == 'HPP_SCENE_1.01                  ':
				self.readScene100(ifile)
			else:
				raise Exception('Invalid header \"' + header + '\"!')
		except:
			ifile.close()
			raise
		ifile.close()

	def readMesh101(self, ifile):
		self.meshes = []

		mesh = hpp_types.Mesh()

		mesh.read(ifile)

		armature_exists = hpp_misc.readUInt8(ifile)
		if armature_exists:
			assert False, 'Not implemented yet!'

		materials_size = hpp_misc.readUInt32(ifile)
		mats = []
		while len(mats) < materials_size:
			assert False, 'Not implemented yet!'

		self.meshes.append(mesh)

	def readScene100(self, ifile):
		assert False, 'Not implemented yet!'
		pass

	def createBlenderObjects(self):
		for mesh in self.meshes:
			mesh.spawn()

def importAndRead(filename):
	importer = Importer()
	importer.read(filename)
	importer.createBlenderObjects()

if __name__ == '__main__':
	Window.FileSelector(importAndRead, 'Export', '*.hppmesh')

