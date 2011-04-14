#!BPY
# coding=UTF-8
"""
Name: 'Hpp Exporter'
Blender: 249
Group: 'Export'
Tooltip: 'Hpp Mesh and Scene Exporter'
"""
import hpp_misc
import hpp_types
import Blender
import bpy
#import copy
#import math
from Blender import Draw, Window, Mathutils
#from Blender.Mathutils import Vector
#from array import array

class SceneExporter:

	def __init__(self):
		self.portals = []
		self.rooms = {}
		self.dobjs = {}
		self.used_meshes = set()
		self.used_mats = set()

	def read(self, export_only_selected, export_materials):
		print 'Export scene'
		print '* Export only selected:', export_only_selected
		print '* Export materials:    ', export_materials

		if export_only_selected:
			objects = Blender.Object.GetSelected()
		else:
			objects = Blender.Object.Get()

		self.portals = []
		self.rooms = {}
		self.dobjs = {}
		self.used_meshes = set()
		self.used_mats = set()

		for obj in objects:
			obj_name = obj.getName()
			# Rooms
			if obj_name[0:4] == 'Room':
				room_name = obj_name[4:]
				if room_name.find('.') != -1:
					raise Exception('Found room that has dot in it\'s name! This is not allowed!')
				self.rooms[room_name] = Room(obj)
				self.used_meshes.add(obj.getData(mesh = True))
			# Portals
			elif obj_name[0:6] == 'Portal':
				portal = Portal(obj)
				self.portals.append(portal)
			# Decoration objects
			elif obj.getType() == 'Mesh':
				dobj = Object(obj)
				self.dobjs[obj_name] = dobj
				self.used_meshes.add(obj.getData(mesh = True))
			else:
				raise Exception('Object \"' + obj.getName() + '\" has unsupported type \"' + obj.getType() + '\"!')

		# Ensure all portals have valid room names
		for portal in self.portals:
			if portal.frontroom_name not in self.rooms:
				raise Exception('Frontroom \"' + portal.frontroom_name + '\" that is referenced in Portal could not be found!')
			if portal.backroom_name not in self.rooms:
				raise Exception('Backroom \"' + portal.backroom_name + '\" that is referenced in Portal could not be found!')

		# Find out used materials
		if export_materials:
			for mesh in self.used_meshes:
				for mat in mesh.materials:
					self.used_mats.add(mat)

	def export(self, filename):

		# Open file and write header
		efile = open(filename, 'wb')
		efile.write('HPP_SCENE_1.00                  ')

		# Portals
		efile.write(hpp_misc.uInt32ToBytes(len(self.portals)))
		for portal in self.portals:
			efile.write(portal.serialize())

		# Rooms
		efile.write(hpp_misc.uInt32ToBytes(len(self.rooms)))
		for room_name, room in self.rooms.items():
			efile.write(hpp_misc.uInt32ToBytes(len(room_name)))
			efile.write(room_name)
			efile.write(room.serialize())

		# Decorative objects
		efile.write(hpp_misc.uInt32ToBytes(len(self.dobjs)))
		for dobj_name, dobj in self.dobjs.items():
			efile.write(hpp_misc.uInt32ToBytes(len(dobj_name)))
			efile.write(dobj_name)
			efile.write(dobj.serialize())

		# Meshes
		efile.write(hpp_misc.uInt32ToBytes(len(self.used_meshes)))
		for mesh in self.used_meshes:
			mymesh = hpp_types.Mesh(mesh)
			efile.write(mymesh.serialize())

		# Materials
		efile.write(hpp_misc.uInt32ToBytes(len(self.used_mats)))
		for mat in self.used_mats:
			mymat = hpp_types.Material(mat)
			efile.write(mymat.serialize())

		efile.close()


class MeshExporter:

	def __init__(self):
		self.mesh = None
		self.armature = None
		self.used_mats = set()

	def read(self, mesh_name, export_armature, export_materials):
		print 'Export mesh'
		print '* Export armature: ', export_armature
		print '* Export materials: ', export_materials

		object = Blender.Object.GetSelected()[0]

		mesh_raw = object.getData(mesh = True)
		self.used_mats = set()

		if export_armature:
			armature_obj = object.getParent()
			assert armature_obj
			assert armature_obj.getType() == 'Armature'
			self.armature = armature_obj.getData()
		else:
			self.armature = None

		# Find out used materials
		if export_materials:
			for mat in mesh_raw.materials:
				self.used_mats.add(mat)

		self.mesh = hpp_types.Mesh(mesh_raw, mesh_name)

	def setMesh(self, mesh, used_mats = set()):
		self.mesh = mesh
		self.used_mats = used_mats

	def export(self, filename):

		# Open file and write header
		efile = open(filename, 'wb')
		efile.write('HPP_MESH_1.00                   ')

		# Mesh
		efile.write(self.mesh.serialize())

		# Armature
		if self.armature:
			efile.write(chr(1))
			myarmature = Armature(self.armature)
			efile.write(myarmature.serialize())
		else:
			efile.write(chr(0))

		# Materials
		efile.write(hpp_misc.uInt32ToBytes(len(self.used_mats)))
		for mat in self.used_mats:
			mymat = Material(mat)
			efile.write(mymat.serialize())

		efile.close()



# ========================================
# ========================================
#
# GUI
#
# ========================================
# ========================================

class SceneExportOptions:
	def __init__(self):
		self.export_only_selected_button = None
		self.export_materials_button = None

class MeshExportOptions:
	def __init__(self):
		self.export_materials_button = None
		self.mesh_name = ''
		self.has_armature = False

# Global options
scene_export_options = SceneExportOptions()
mesh_export_options = MeshExportOptions()
close_window = False

def pushCancelExportScene(event, value):
	global close_window
	close_window = True

def pushCancelExportMesh(event, value):
	global close_window
	close_window = True

def pushExportScene(event, value):
	global close_window
	global scene_export_options

	close_window = True

	export_only_selected = scene_export_options.export_only_selected_button.val
	export_materials = scene_export_options.export_materials_button.val
	try:
		exporter = SceneExporter()
		exporter.read(export_only_selected, export_materials)
		Window.FileSelector(exporter.export, 'Export')
	except Exception, e:
		print 'ERROR:', e
		Draw.PupMenu('Error%t|' + str(e))

def modifyExportMeshName(event, value):
	global mesh_export_options
	mesh_export_options.mesh_name = mesh_export_options.mesh_name_input.val

def pushExportMesh(event, value):
	global close_window
	global mesh_export_options

	close_window = True

	export_materials = mesh_export_options.export_materials_button.val
	export_armature = mesh_export_options.has_armature and mesh_export_options.export_armature_button.val
	mesh_name = mesh_export_options.mesh_name
	try:
		exporter = MeshExporter()
		exporter.read(mesh_name, export_armature, export_materials)
		Window.FileSelector(exporter.export, 'Export')
	except Exception, e:
		print 'ERROR:', e
		Draw.PupMenu('Error%t|' + str(e))

def buildSceneExportGui():

	global scene_export_options

	scr_size = Window.GetScreenSize()
	scr_c_x = scr_size[0] / 2
	scr_c_y = scr_size[1] / 2

	# Topic
	Draw.Label('Export scene', scr_c_x - 120, scr_c_y + 35, 240, 20)

	# Only selected
	scene_export_options.export_only_selected_button = Draw.Toggle('Only selected', 0, scr_c_x - 120, scr_c_y + 5, 240, 20, False, 'Should only selected objects be exported')

	# Should materials be included
	scene_export_options.export_materials_button = Draw.Toggle('Export materials in use', 0, scr_c_x - 120, scr_c_y - 25, 240, 20, True, 'Should those materials be included that are used by scene')

	# Actions
	Draw.PushButton('Cancel', 1, scr_c_x - 120, scr_c_y - 55, 120, 20, '', pushCancelExportScene)
	Draw.PushButton('Export', 1, scr_c_x, scr_c_y - 55, 120, 20, '', pushExportScene)

def buildMeshExportGui():

	global mesh_export_options

	scr_size = Window.GetScreenSize()
	scr_c_x = scr_size[0] / 2
	scr_c_y = scr_size[1] / 2

	draw_y = scr_c_y + 35
	if mesh_export_options.has_armature:
		draw_y += 15

	# Topic
	Draw.Label('Export mesh', scr_c_x - 120, draw_y, 240, 20)
	draw_y -= 30

	# Name
	mesh_export_options.mesh_name_input = Draw.String('Name: ', 1, scr_c_x - 120, draw_y, 240, 20, mesh_export_options.mesh_name, 399, '', modifyExportMeshName)
	draw_y -= 30

	# Should armature be included
	if mesh_export_options.has_armature:
		mesh_export_options.export_armature_button = Draw.Toggle('Export armature and actions', 0, scr_c_x - 120, draw_y, 240, 20, True, 'Should armature and actions of mesh be exported too')
		draw_y -= 30

	# Should materials be included
	mesh_export_options.export_materials_button = Draw.Toggle('Export materials in use', 0, scr_c_x - 120, draw_y, 240, 20, True, 'Should those materials be included that are used by mesh')
	draw_y -= 30

	# Actions
	Draw.PushButton('Cancel', 1, scr_c_x - 120, draw_y, 120, 20, '', pushCancelExportMesh)
	Draw.PushButton('Export', 1, scr_c_x, draw_y, 120, 20, '', pushExportMesh)
	draw_y -= 30

def doSceneExportGui():
	while not close_window:
		Draw.UIBlock(buildSceneExportGui, 0)

def doMeshExportGui(mesh_name, has_armature):
	global mesh_export_options
	mesh_export_options.mesh_name = mesh_name
	mesh_export_options.has_armature = has_armature
	while not close_window:
		Draw.UIBlock(buildMeshExportGui, 0)

if __name__ == '__main__':

	global close_window
	
	# Export choices
	options = '|Scene|Material(s)'

	# Check if it is possible to export Mesh
	objects = Blender.Object.GetSelected()
	if len(objects) == 1 and objects[0].getType() == 'Mesh':
		options += '|Mesh'

	export_type = Draw.PupMenu('What would you like to export?%t' + options)

	if export_type == 1:
		close_window = False
		doSceneExportGui()
	elif export_type == 3:
		close_window = False
		mesh_name = objects[0].getName()
		mesh_parent = objects[0].getParent()
		has_armature = (mesh_parent and mesh_parent.getType() == 'Armature')
		doMeshExportGui(mesh_name, has_armature)

