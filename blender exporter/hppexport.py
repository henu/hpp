# coding=UTF-8

bl_info = {
	'name' : 'LibHPP mesh and scene exporter',
	'author' : 'Henrik Heino <henu@henu.fi>',
	'version' : (1, 0, 1),
	'blender' : (2, 5, 8),
	'api' : 37702,
	'location' : 'File > Export > LibHPP',
	'description' : 'Export meshes and scenes to LibHPP format.',
	'warning' : '',
	'wiki_url' : '',
	'tracker_url' : '',
	'category' : 'Import-Export'
}

import hpp_misc
import hpp_types

import bpy



# ========================================
# ========================================
#
# Main exporter module
#
# ========================================
# ========================================

ExportModes = ( ('MESH', 'Mesh', 'A single mesh.'), ('SCENE', 'Scene', 'Multiple objects as a scene.') )

class HppExporter(bpy.types.Operator):

	bl_idname = 'export.libhpp'
	bl_label = 'Export LibHPP'

	# Options
	filepath = bpy.props.StringProperty(name = 'File Path', description = 'Path of file to export', maxlen = 1024, subtype='FILE_PATH')
	opts_export_mode = bpy.props.EnumProperty(name = 'Export', description = 'Select exporting method.', items = ExportModes, default = 'MESH')
	opts_mesh_export_materials = bpy.props.BoolProperty(name = 'Mesh / Export materials', description = 'Include used materials.', default = False)
	opts_mesh_name = bpy.props.StringProperty(name = 'Mesh / Custom name', description = 'Custom name instead of default one. Leave empty for default.', default = '')
	opts_mesh_has_armature = bpy.props.BoolProperty(name = 'Mesh / Export armature and actions', description = 'Include armature and actions of object.', default = False)
	opts_scene_export_only_selected = bpy.props.BoolProperty(name = 'Scene / Export only selected', description = 'Exports only selected objects.', default = False)
	opts_scene_export_materials = bpy.props.BoolProperty(name = 'Scene / Export materials', description = 'Include used materials.', default = False)

	def execute(self, context):
		if len(self.filepath) == 0:
			raise Exception('Filename cannot be empty!')

		if self.opts_export_mode == 'MESH':
			# Fix filename
			self.filepath = bpy.path.ensure_ext(self.filepath, ".hppmesh")
			# Do exporting
			exporter = MeshExporter()
			exporter.read(self.opts_mesh_name, self.opts_mesh_has_armature, self.opts_mesh_export_materials)
			exporter.export(self.filepath)
		else:
			# Fix filename
			self.filepath = bpy.path.ensure_ext(self.filepath, ".hppscene")
			# Do exporting
			exporter = SceneExporter()
			exporter.read(self.opts_scene_export_only_selected, self.opts_scene_export_materials)
			exporter.export(self.filepath)

		return {'FINISHED'}

	def invoke(self, context, event):
		wm = context.window_manager
		wm.fileselect_add(self)
		return {'RUNNING_MODAL'}



# ========================================
# ========================================
#
# Register module
#
# ========================================
# ========================================

def menu_func(self, context):
	self.layout.operator(HppExporter.bl_idname, text='LibHPP scene or mesh')

def register():
	bpy.utils.register_module(__name__)
	bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
	bpy.utils.unregister_module(__name__)
	bpy.types.INFO_MT_file_export.remove(menu_func)



# ========================================
# ========================================
#
# Mesh exporter module
#
# ========================================
# ========================================

class MeshExporter:

	def __init__(self):
		self.mesh = None
		self.armature = None
		self.used_mats = set()

	def read(self, mesh_name, export_armature, export_materials):
		print('Reading mesh for exporting...')
		print('* Custom name     : ' + mesh_name)
		print('* Export armature : ' + str(export_armature))
		print('* Export materials: ' + str(export_materials))

		object = hpp_misc.getObjects(True)[0]

		mesh_raw = object.data
		self.used_mats = set()

		if export_armature:
			armature_obj = object.parent
			assert armature_obj
			assert armature_obj.type == 'ARMATURE'
			self.armature = armature_obj.data
		else:
			self.armature = None

		# Find out used materials
		if export_materials:
			for mat in mesh_raw.materials:
				self.used_mats.add(mat)

		self.mesh = hpp_types.Mesh(object, mesh_name)
		print('Reading done!')

	def setMesh(self, mesh, used_mats = set()):
		self.mesh = mesh
		self.used_mats = used_mats

	def export(self, filename):
		print('Writing mesh to file \"' + filename + '\"...')

		# Open file and write header
		efile = open(filename, 'wb')
		efile.write(bytes('HPP_MESH_1.01                   ', 'ASCII'))

		# Mesh
		efile.write(self.mesh.serialize())

		# Armature
		if self.armature:
			efile.write(bytes([1]))
			myarmature = hpp_types.Armature(self.armature)
			efile.write(myarmature.serialize())
		else:
			efile.write(bytes([0]))

		# Materials
		efile.write(hpp_misc.uInt32ToBytes(len(self.used_mats)))
		for mat in self.used_mats:
			mymat = hpp_types.Material(mat)
			efile.write(mymat.serialize())

		efile.close()
		print('Writing done!')



# ========================================
# ========================================
#
# Scene exporter module
#
# ========================================
# ========================================

class SceneExporter:

	def __init__(self):
		self.portals = []
		self.rooms = {}
		self.dobjs = {}
		self.used_meshes = set()
		self.used_mats = set()

	def read(self, export_only_selected, export_materials):
		print('Reading scene for exporting...')
		print('* Export only selected: ' + str(export_only_selected))
		print('* Export materials:     ' + str(export_materials))

		objects = hpp_misc.getObjects(export_only_selected)

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
			elif obj.type == 'MESH':
				dobj = Object(obj)
				self.dobjs[obj_name] = dobj
				self.used_meshes.add(obj.getData(mesh = True))
			else:
				raise Exception('Object \"' + obj.getName() + '\" has unsupported type \"' + obj.type + '\"!')

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
		print('Reading done!')

	def export(self, filename):
		print('Writing scene to file \"' + filename + '\"...')

		# Open file and write header
		efile = open(filename, 'wb')
		efile.write(bytes('HPP_SCENE_1.01                  ', 'ASCII'))

		# Portals
		efile.write(hpp_misc.uInt32ToBytes(len(self.portals)))
		for portal in self.portals:
			efile.write(portal.serialize())

		# Rooms
		efile.write(hpp_misc.uInt32ToBytes(len(self.rooms)))
		for room_name, room in self.rooms.items():
			efile.write(hpp_misc.stringToBytes(room_name))
			efile.write(room.serialize())

		# Decorative objects
		efile.write(hpp_misc.uInt32ToBytes(len(self.dobjs)))
		for dobj_name, dobj in self.dobjs.items():
			efile.write(hpp_misc.stringToBytes(dobj_name))
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
		print('Writing done!')

