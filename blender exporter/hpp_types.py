# coding=UTF-8
import hpp_misc
import mathutils
import copy

class MeshVertex:
	def __init__(self):
		self.pos = mathutils.Vector([0, 0, 0])
		self.vgroups = {}

	def __deepcopy__(self, memo):
		newvrt = MeshVertex()
		newvrt.pos = mathutils.Vector(self.pos)
		newvrt.vgroups = copy.deepcopy(self.vgroups, memo)
		return newvrt

	def serialize(self):
		result = bytes()
		result += hpp_misc.vectorToBytes(self.pos)
		result += hpp_misc.uInt32ToBytes(len(self.vgroups))
		for vgroup_id, weight in self.vgroups.items():
			result += hpp_misc.uInt32ToBytes(vgroup_id)
			result += hpp_misc.floatToBytes(weight)
		return result

	def read(self, ifile):
		self.pos = mathutils.Vector([0, 0, 0])
		self.vgroups = {}

		self.pos = hpp_misc.readVector(ifile, 3)
		vgroups_size = hpp_misc.readUInt32(ifile)
		while len(self.vgroups) < vgroups_size:
			vgroup_id = hpp_misc.readUInt32(ifile)
			weight = hpp_misc.readFloat(ifile)
			assert vgroup_id not in self.vgroups, 'Vertexgroup #' + str(vgroup_id) + 'defined more than once for Vertex!'
			self.vgroups[vgroup_id] = weight

class MeshFace:

	# Halving styles
	V0_TO_V2 = 0
	V1_TO_V3 = 1
	SHORTER = 2
	LONGER = 3

	def __init__(self, vrts_len):
		self.vrts = []
		self.uvs = []
		while len(self.uvs) < vrts_len: self.uvs.append([])
		self.smooth = False

	def __deepcopy__(self, memo):
		newface = MeshFace(0)
		newface.vrts = copy.deepcopy(self.vrts, memo)
		for uvlayer in self.uvs:
			newface_uvlayer = []
			for uv in uvlayer:
				newface_uvlayer.append(mathutils.Vector(uv))
			newface.uvs.append(newface_uvlayer)
		newface.smooth = self.smooth
		return newface;

	def getUVComponentCounts(self):
		result = []
		assert len(self.uvs) == len(self.vrts), 'Vertex and UV counts differ!'
		layers = len(self.uvs[0])
		for layer in range(0, layers):
			result.append(len(self.uvs[0][layer]))
		return result

	def serialize(self):
		result = bytes()

		# Vertices. Count is not needed, because this is called for
		# triangles and quads.
		for vrt in self.vrts:
			result += hpp_misc.uInt32ToBytes(vrt)

		# UVs
		assert len(self.uvs) == len(self.vrts), 'Vertex and UV counts differ!'
		vrts_len = len(self.vrts)
		layers = len(self.uvs[0])
		for layer_id in range(0, layers):
			for vrt_id in range(0, vrts_len):
				result += hpp_misc.vectorToBytes(self.uvs[vrt_id][layer_id])

		# Smooth
		result += bytes([self.smooth])

		return result

	def read(self, ifile, vrts_len, uvlayers_compcounts):
		self.vrts = []
		self.uvs = []
		self.smooth = False

		# Vertices. Also init UV containers
		for vrt_id in range(0, vrts_len):
			self.vrts.append(hpp_misc.readUInt32(ifile))
			self.uvs.append([])

		# UVs
		layers = len(uvlayers_compcounts)
		for layer_id in range(0, layers):
			for vrt_id in range(0, vrts_len):
				uv = hpp_misc.readVector(ifile, uvlayers_compcounts[layer_id])
				self.uvs[vrt_id].append(uv)

		# Smooth
		self.smooth = (hpp_misc.readUInt8(ifile) != 0)

	# Halves quad to two triangles.
	def halveQuad(self, halvingstyle = SHORTER, vrts = None):
		# Decide halving style
		from_v0_to_v2 = None
		if halvingstyle == MeshFace.V0_TO_V2:
			from_v0_to_v2 = True
		elif halvingstyle == MeshFace.V1_TO_V3:
			from_v0_to_v2 = False
		else:
			assert vrts != None, 'vrts must be given!'
			diff_v0_to_v2 = vrts[self.vrts[0]].pos - vrts[self.vrts[2]].pos
			diff_v1_to_v3 = vrts[self.vrts[1]].pos - vrts[self.vrts[3]].pos
			if diff_v0_to_v2.length < diff_v1_to_v3.length:
				from_v0_to_v2 = (halvingstyle == MeshFace.SHORTER)
			else:
				from_v0_to_v2 = (halvingstyle == MeshFace.LONGER)

		# Do halving
		newtri1 = MeshFace(3)
		newtri2 = MeshFace(3)
		# Other options
		newtri1.smooth = self.smooth
		newtri2.smooth = self.smooth
		# Vertices and UVs
		newtri1.vrts.append(self.vrts[0])
		newtri1.vrts.append(self.vrts[1])
		newtri2.vrts.append(self.vrts[2])
		newtri2.vrts.append(self.vrts[3])
		if len(self.uvs[0]) > 0:
			newtri1.uvs[0] = self.uvs[0]
			newtri1.uvs[1] = self.uvs[1]
			newtri2.uvs[0] = self.uvs[2]
			newtri2.uvs[1] = self.uvs[3]
		if from_v0_to_v2:
			newtri1.vrts.append(self.vrts[2])
			newtri2.vrts.append(self.vrts[0])
			if len(self.uvs[0]) > 0:
				newtri1.uvs[2] = self.uvs[2]
				newtri2.uvs[2] = self.uvs[0]
		else:
			newtri1.vrts.append(self.vrts[3])
			newtri2.vrts.append(self.vrts[1])
			if len(self.uvs[0]) > 0:
				newtri1.uvs[2] = self.uvs[3]
				newtri2.uvs[2] = self.uvs[1]
		return [newtri1, newtri2]


class MeshSubmesh:
	def __init__(self):
		self.uvlayers_compcounts = []
		self.tris = []
		self.quads = []
		self.mat = ''

	def serialize(self):
		result = bytes()

		# Component counts of UV layers
		result += hpp_misc.uInt32ToBytes(len(self.uvlayers_compcounts))
		for compcount in self.uvlayers_compcounts:
			result += hpp_misc.uInt8ToBytes(compcount)

		# Triangles
		result += hpp_misc.uInt32ToBytes(len(self.tris))
		for tri in self.tris:
			result += tri.serialize()

		# Quads
		result += hpp_misc.uInt32ToBytes(len(self.quads))
		for quad in self.quads:
			result += quad.serialize()

		# Material
		if not self.mat:
			result += hpp_misc.uInt32ToBytes(0)
		else:
			result += hpp_misc.stringToBytes(self.mat)

		return result

	def read(self, ifile):

		self.uvlayers_compcounts = []
		self.tris = []
		self.quads = []
		self.mat = ''

		# Component counts of UV layers
		layers_size = hpp_misc.readUInt32(ifile)
		while len(self.uvlayers_compcounts) < layers_size:
			self.uvlayers_compcounts.append(hpp_misc.readUInt8(ifile))

		# Triangles
		tris_size = hpp_misc.readUInt32(ifile)
		while len(self.tris) < tris_size:
			new_tri = MeshFace(3)
			new_tri.read(ifile, 3, self.uvlayers_compcounts)
			self.tris.append(new_tri)

		# Quads
		quads_size = hpp_misc.readUInt32(ifile)
		while len(self.quads) < quads_size:
			new_quad = MeshFace(4)
			new_quad.read(ifile, 4, self.uvlayers_compcounts)
			self.quads.append(new_quad)

		# Material
		mat_size = hpp_misc.readUInt32(ifile)
		self.mat = ifile.read(mat_size)

class MeshCutError(Exception):
	def __init__(self, reason):
		self.reason = reason
	def __str__(self):
		return str(self.reason)

class Mesh:

	def __init__(self, object = None, custom_name = ''):

		# If object is not given, then construct dummy Mesh
		if not object:
			return

		if object.type != 'MESH':
			raise Exception('Mesh was tried to construct with object that is not a mesh!')

		# Get mesh and vertex groups from object
		mesh = object.data
		self.vgroups = []
		for vertex_group in object.vertex_groups:
			self.vgroups.append(vertex_group.name)

		# Read name
		if custom_name == '':
			self.name = mesh.name
		else:
			self.name = custom_name

		# Read vertices
		self.vrts = []
		for vert in mesh.vertices:
			new_vert = MeshVertex()
			new_vert.pos = mathutils.Vector(vert.co)
			vgroups = vert.groups
			for vgroup in vgroups:
				vgroup_id = vgroup.group
				weight = vgroup.weight
				assert vgroup_id not in new_vert.vgroups, 'Vertex group ID already found!'
				new_vert.vgroups[vgroup_id] = weight
			self.vrts.append(new_vert)

		# Read materials
		self.submeshes = []
		for mat in mesh.materials:
			new_submesh = MeshSubmesh()
			new_submesh.mat = mat.name
			new_submesh.uvlayers_compcounts = None
			self.submeshes.append(new_submesh)
		# If there was not any materials, then create submesh with no
		# material.
		if not self.submeshes:
			new_submesh = MeshSubmesh()
			new_submesh.mat = None
			new_submesh.uvlayers_compcounts = None
			self.submeshes.append(new_submesh)

		# Get UVs from mesh
		# TODO: Here we get all UV layers, but is this done correctly?
		mesh_uv_layers = []
		for uv in mesh.uv_textures:
			mesh_uv_layers.append(uv.data)

		# Read faces
		face_id = 0
		for face in mesh.faces:
			face_verts_len = len(face.vertices)
			new_face = MeshFace(face_verts_len)
			# Vertices
			for vert in face.vertices:
				new_face.vrts.append(vert)
			# UVs
			for mesh_uv_layer in mesh_uv_layers:
				face_uv_layer = mesh_uv_layer[face_id]
				vrt_id = 0
				for uv in face_uv_layer.uv:
					new_face.uvs[vrt_id].append(mathutils.Vector(uv))
					vrt_id += 1
			# Smooth
			new_face.smooth = bool(face.use_smooth)

			# Store face to submesh
			submesh = self.submeshes[face.material_index]
			if face_verts_len == 3:
				submesh.tris.append(new_face)
			elif face_verts_len == 4:
				submesh.quads.append(new_face)
			else:
				raise Exception('Mesh \"' + mesh.name + '\" has face with invalid number(' + str(face_verts_len) + ') of vertices!')

			# Check if uv layer count has been set for this submesh
			if submesh.uvlayers_compcounts == None:
				submesh.uvlayers_compcounts = new_face.getUVComponentCounts()
			else:
				if submesh.uvlayers_compcounts != new_face.getUVComponentCounts():
					raise Exception('Mesh \"' + mesh.name + '\" has submesh with faces using different amount of uv components!')
			face_id += 1

	def __deepcopy__(self, memo):
		newmesh = Mesh()
		newmesh.name = copy.deepcopy(self.name, memo)
		newmesh.vrts = copy.deepcopy(self.vrts, memo)
		newmesh.vgroups = copy.deepcopy(self.vgroups, memo)
		newmesh.submeshes = copy.deepcopy(self.submeshes, memo)
		return newmesh;

	def createNewSubmesh(self, mat, uvlayers_compcounts):
		newsubmesh = MeshSubmesh()
		newsubmesh.mat = copy.deepcopy(mat)
		newsubmesh.uvlayers_compcounts = copy.deepcopy(uvlayers_compcounts)
		self.submeshes.append(newsubmesh)
		return len(self.submeshes) - 1

	def convertQuadsToTris(self, halvingstyle = MeshFace.SHORTER):
		for submesh in self.submeshes:
			for quad in submesh.quads:
				submesh.tris += quad.halveQuad(halvingstyle, self.vrts)

			# Clear quads
			submesh.quads = []

	# TODO: Support quads!
	def isConvex(self):
		for submesh in self.submeshes:
			assert len(submesh.quads) == 0, 'Quads not supported yet!'
			for tri in submesh.tris:
				# Get normal of triangle
				v0 = self.vrts[tri.vrts[0]].pos
				v1 = self.vrts[tri.vrts[1]].pos
				v2 = self.vrts[tri.vrts[2]].pos
				normal = (v1 - v0).cross(v2 - v0)
				# Now go all vertices through, and ensure they are at the back of this face.
				for vrt in self.vrts:
					dp = normal.dot(vrt.pos - v0)
					if dp > 0.001:
						return False
		return True

	# Returns hits to faces in depth order. List contains tuples. First
	# part of tuple is collision point and second is normal of face.
	def getRayHits(self, ray_begin, ray_dir, halvingstyle = MeshFace.SHORTER):
		result = []
		for submesh in self.submeshes:
			# Do temporary conversion from quads to meshes
			qtris = []
			for quad in submesh.quads:
				qtris += quad.halveQuad(halvingstyle, self.vrts)
			# Check triangles
			for tri in (submesh.tris + qtris):
				pos0 = self.vrts[tri.vrts[0]].pos
				pos1 = self.vrts[tri.vrts[1]].pos
				pos2 = self.vrts[tri.vrts[2]].pos
				hits, hit_pos, hit_normal = hpp_misc.rayHitsTriangle(ray_begin, ray_dir, pos0, pos1, pos2)
				if (hits):
					result.append((hit_pos, hit_normal))
		# Sort by depth
		result = sorted(result, hpp_misc.DistComparer(ray_begin, 0))
		return result

	# Merges vertices at straight lines if it can be done without changing
	# the appearance of Mesh. This means, that UV coordinates, face
	# positions and vertex groups are preserved.
	# TODO: Code support for quads!
	def mergeUselessVertices(self):
		assert self.isClosed()
		MAX_DIFF = 0.001
		MAX_DIFF_TO_2 = MAX_DIFF * MAX_DIFF
		MAX_NORMAL_CHANGE = 0.01
		do_merging = True
		while do_merging:
			do_merging = False
			vrt_id = 0
			while vrt_id < len(self.vrts):
				vrt = self.vrts[vrt_id]
				vrt_faces = []
				vrt_pos = vrt.pos
				# Begin by finding all neighbor vertices of this vertex
				# and all faces that are connected to this vertex.
				ngbs_set = set()
				for submesh_id in range(0, len(self.submeshes)):
					submesh = self.submeshes[submesh_id]
					assert len(submesh.quads) == 0
					for face_id in range(0, len(submesh.tris)):
						face = submesh.tris[face_id]
						if vrt_id in face.vrts:
							face_ident = submesh_id, face_id
							vrt_faces.append(face_ident)
							for ngb in face.vrts:
								if ngb != vrt_id:
									ngbs_set.add(ngb)
				# Move to list
				ngbs = list(ngbs_set)
				# Now compare all neighbors against each others to find
				# out if this vertices is nicely between them. Nice means
				# that merging it would not change appearance of mesh.
				best_ngb1 = -1
				best_ngb2 = -1
				best_ngbs_diff = MAX_DIFF * 2
				best_ngbselect = -1
				for n1_offset in range(0, len(ngbs)):
					n1_id = ngbs[n1_offset]
					for n2_offset in range(n1_offset+1, len(ngbs)):
						n2_id = ngbs[n2_offset]
						n1_pos = self.vrts[n1_id].pos
						n2_pos = self.vrts[n2_id].pos
						n1_to_n2 = n2_pos - n1_pos
						# If neighbors are too close to each others, then skip these.
						if n1_to_n2.length == 0.0:
							continue
						# Calculate how much neighbor two would affect to this vertex.
						n2_weight = hpp_misc.distanceToPlane(vrt_pos, n1_pos, n1_to_n2)
						# Skip is vertex is not between these two neighbors.
						if n2_weight < 0.0 or n2_weight > 1.0:
							continue
						# Calculate position at ray
						vrt_pos_ray = n1_pos + n2_weight * n1_to_n2
						# Calculate distance to the ray
						diff = (vrt_pos_ray - vrt_pos).length
						# If this diff would not be the best, then give up immediately
						if diff > MAX_DIFF or diff > best_ngbs_diff:
							continue

						# Start to check to which ends this vertex could be merged.
						n1_possible = True
						n2_possible = True

						# Check if there are any triangles that
						# would change its normal too much
						for face_ident in vrt_faces:
							face = self.submeshes[face_ident[0]].tris[face_ident[1]]
							# Calculate current normal of face
							face_vposs = []
							for corner in range(0, len(face.vrts)):
								fvrt_id = face.vrts[corner]
								if fvrt_id == vrt_id:
									vrt_corner = corner
								face_vposs.append(self.vrts[fvrt_id].pos)
							face_nrm = (face_vposs[1] - face_vposs[0]).cross(face_vposs[2] - face_vposs[0])
							# Now check what would be the normal if
							# vertex would be merged to another end
							if n1_possible:
								face_vposs[vrt_corner] = n1_pos
								face_nrm_check = (face_vposs[1] - face_vposs[0]).cross(face_vposs[2] - face_vposs[0])
								if face_nrm_check.length < 0.001:
									# Face has gone to zero sized. This is only
									# allowed, if this face has that neighbor
									# as a vertex, that we are now merging to.
									if n1_id not in face.vrts:
										n1_possible = False
								else:
									n1_possible = mathutils.AngleBetweenVecs(face_nrm, face_nrm_check) < MAX_NORMAL_CHANGE
							if n2_possible:
								face_vposs[vrt_corner] = n2_pos
								face_nrm_check = (face_vposs[1] - face_vposs[0]).cross(face_vposs[2] - face_vposs[0])
								if face_nrm_check.length < 0.001:
									# Face has gone to zero sized. This is only
									# allowed, if this face has that neighbor
									# as a vertex, that we are now merging to.
									if n2_id not in face.vrts:
										n2_possible = False
								else:
									n2_possible = mathutils.AngleBetweenVecs(face_nrm, face_nrm_check) < MAX_NORMAL_CHANGE
							# TODO: What if face becomes zero area'd?!
							# If merging to both ends have become
							# impossible, then give up.
							if not n1_possible and not n2_possible:
								break

						# Time to give up?
						if not n1_possible and not n2_possible:
							break

						# No go through all edges that are connected
						# to this vertex and not to the neighbors.
						# They need to be ensured that they don't
						# separate two faces that have either
						# different submesh or that have different
						# smooth/solid setting.
						for face_ident in vrt_faces:
							face = self.submeshes[face_ident[0]].tris[face_ident[1]]
						for ngb in ngbs_set:
							# Skip those neighbors that are now being tested.
							if ngb == n1_id or ngb == n2_id:
								continue
							# Go faces through and test those faces
							# that are separated by this edge.
							faceprops = []
							for face_ident in vrt_faces:
								face = self.submeshes[face_ident[0]].tris[face_ident[1]]
								if ngb in face.vrts:
									faceprops.append([face.smooth, face_ident[0]])
									if len(faceprops) == 2:
										break
							# Now check if these faces have same properties
							assert len(faceprops) == 2, 'Not all faces could be found!'
							if faceprops[0] != faceprops[1]:
								n1_possible = False
								n2_possible = False
								break

						# Time to give up?
						if not n1_possible and not n2_possible:
							break

						# Ensure UV coordinates will not change
						# if merging is done. This is done by
						# going both faces through from
						# different sides of merging line.
						assert n1_id != vrt_id, 'Neighbor cannot be same that merging vertex is!'
						next_vrts = [n1_id, n1_id]
						vrt_uvs = []
						n1_uvs = []
						n2_uvs = [None, None]
						facesides = {}
						loop_num = 0
						uvs_failed = False
						handled_face_idents = set()
						while (next_vrts[0] != n2_id or next_vrts[1] != n2_id) and not uvs_failed:
							loop_num += 1
							# Find faces that contains wanted vertices
							new_next_vrts = []
							vrt_corners = []
							faces = []
							# Search new faces in order
							while len(new_next_vrts) < 2:
								search_vrt = next_vrts[len(new_next_vrts)]
								# If ending vertex is tried to search, then skip this.
								if search_vrt == n2_id:
									new_next_vrts.append(n2_id)
									vrt_corners.append(-1)
									faces.append(None)
									continue
								# Do searching
								for face_ident in vrt_faces:
									# Skip already handled face identifications
									# TODO: Would it be possible to remove this face_ident from set?
									if face_ident in handled_face_idents:
										continue
									face = self.submeshes[face_ident[0]].tris[face_ident[1]]
									if search_vrt in face.vrts:
										handled_face_idents.add(face_ident)
										faces.append(face)
										# Get side
										side = len(new_next_vrts)
										assert face_ident not in facesides, 'Side of this face is already defined!'
										facesides[face_ident] = side
										assert vrt_id in face.vrts, 'Merging vertex is not found!'
										assert vrt_id != search_vrt, 'Cannot search merging vertex!'
										for corner in range(0, len(face.vrts)):
											vrt2_id = face.vrts[corner]
											if vrt2_id == vrt_id:
												vrt_corners.append(corner)
											elif vrt2_id != search_vrt:
												new_next_vrts.append(vrt2_id)
												if vrt2_id == n2_id:
													n2_uvs[side] = face.uvs[corner]
											else:
												assert vrt2_id == search_vrt, 'Not correct vertex!'
												if loop_num == 1:
													n1_uvs.append(face.uvs[corner])
										break
								assert len(new_next_vrts) == len(vrt_corners), 'List sizes differ!'
								assert len(faces) == len(vrt_corners), 'List sizes differ!'
							assert len(faces) == 2, 'No faces found!'
							# Now we have found next vertices. We now
							# have a) previous vertex, b) next vertex
							# and c) our current vertex (THE vertex).
							# This has been done to both sides of
							# mergeline.
							# First check UVs
							for side in range(0, 2):
								# If this side has ended, then skip
								if not faces[side]:
									continue
								uvs = faces[side].uvs[vrt_corners[side]]
								uvs_diff = 0.0
								if len(vrt_uvs) <= side:
									new_uvs = []
									for uv in uvs:
										new_uvs.append(mathutils.Vector(uv))
									vrt_uvs.append(new_uvs)
								else:
									assert len(uvs) == len(vrt_uvs[side]), 'Found faces in same submesh, that have different number of layers in UVs!'
									for layer in range(0, len(uvs)):
										assert len(uvs[layer]) == len(vrt_uvs[side][layer]), 'Found faces in same submesh, that have different UV components in layer ' + str(layer) + '!'
										for component in range(0, len(uvs[layer])):
											component_diff = uvs[layer][component] - vrt_uvs[side][layer][component]
											uvs_diff += component_diff * component_diff
									# If difference is too big, then mark this as failed
									if uvs_diff > MAX_DIFF_TO_2:
										uvs_failed = True
										break

							next_vrts = new_next_vrts

						# Time to give up?
						if uvs_failed:
							break

						assert len(vrt_uvs) == 2, 'No UVs defined!'

						# Finally ensure found UVs can be formulated using found ends.
						# TODO: Ensure vertegroups are okay too!
						n1_weight = 1.0 - n2_weight
						for side in range(0, 2):
							assert n2_uvs[side] != None, 'No UVs for neighbor #2 found!'
							assert len(n1_uvs[side]) == len(vrt_uvs[side]), 'UVs does not match!'
							assert len(n2_uvs[side]) == len(vrt_uvs[side]), 'UVs does not match!'
							for layer in range(0, len(vrt_uvs[side])):
								assert len(n1_uvs[side][layer]) == len(vrt_uvs[side][layer]), 'UV components does not match at layer ' + str(layer) + '!'
								assert len(n2_uvs[side][layer]) == len(vrt_uvs[side][layer]), 'UV components does not match at layer ' + str(layer) + '!'
								formed_uv = n1_uvs[side][layer] * n1_weight + n2_uvs[side][layer] * n2_weight
								if (formed_uv - vrt_uvs[side][layer]).length > MAX_DIFF:
									uvs_failed = True
									break
							if uvs_failed:
								break

						# Time to give up?
						if uvs_failed:
							break

						# Store this neighbor combination as a new best one
						best_ngb1 = n1_id
						best_ngb2 = n2_id
						best_ngbs_diff = diff
						best_facesides = facesides
						if n1_possible and (n1_weight < n2_weight or not n2_possible):
							best_ngbselect = n1_id
							best_uvs = n1_uvs
						else:
							assert n2_possible, 'Merging should not be possible!'
							best_ngbselect = n2_id
							best_uvs = n2_uvs

				# Now all combinations have been tested and if
				# something is found, it should be the best.
				if best_ngbselect >= 0:
					do_merging = True
					# Since this vertex is merged to another one of
					# neighbors, it means that all faces of this
					# vertex, and no other faces, are being
					# modified, so go them through now.
					# Also, two of faces will be destroyed because
					# of this merge.
					destroyable_faces = []
					for face_ident in vrt_faces:
						face = self.submeshes[face_ident[0]].tris[face_ident[1]]
						assert face_ident in best_facesides, 'No side definition of face found!'
						side = best_facesides[face_ident]
						if best_ngbselect in face.vrts:
							destroyable_faces.append(face_ident)
							continue
						for corner in range(0, len(face.vrts)):
							if face.vrts[corner] == vrt_id:
								vrt_corner = corner
						face.vrts[vrt_corner] = best_ngbselect
						face.uvs[vrt_corner] = best_uvs[side]

					# Destroy destroyable faces
					assert len(destroyable_faces) == 2, 'Expecting two destroyable faces, but ' + str(len(destroyable_faces)) + ' was found!'
					d_submesh1_id = destroyable_faces[0][0]
					d_submesh2_id = destroyable_faces[1][0]
					d_face1_id = destroyable_faces[0][1]
					d_face2_id = destroyable_faces[1][1]
					if d_submesh1_id == d_submesh2_id and d_face1_id > d_face2_id:
						self.submeshes[d_submesh1_id].tris.pop(d_face1_id)
						self.submeshes[d_submesh2_id].tris.pop(d_face2_id)
					else:
						self.submeshes[d_submesh2_id].tris.pop(d_face2_id)
						self.submeshes[d_submesh1_id].tris.pop(d_face1_id)

					# Destroy this vertex
					self.vrts.pop(vrt_id)

					# Go all faces through and fix pointings to vertices
					for submesh in self.submeshes:
						for face in (submesh.tris + submesh.quads):
							for corner in range(0, len(face.vrts)):
								assert face.vrts[corner] != vrt_id, 'Usage of destroyed vertex found!'
								if face.vrts[corner] > vrt_id:
									face.vrts[corner] -= 1

				else:
					vrt_id += 1

	# Separates loose parts. That is, parts that are not connected by any
	# faces. One loose part will be left as this Mesh.
	def separateLooseParts(self):
		faces_left = 0
		for submesh in self.submeshes:
			faces_left += len(submesh.tris) + len(submesh.quads)
		# Set that will contain tuples that keep track which faces has
		# been assigned to some new loose part.
		#handled_faces = set()
		loose_meshes = []
		while faces_left > 0:
			faces_to_check = []
			vrt_map = {}
			new_vrts = []
			new_submeshes = []
			for submesh in self.submeshes:
				new_submesh = MeshSubmesh()
				new_submesh.uvlayers_compcounts = copy.deepcopy(submesh.uvlayers_compcounts)
				new_submesh.mat = copy.deepcopy(submesh.mat)
				new_submeshes.append(new_submesh)
			# Initial checking of new face
			for submesh_id in range(0, len(self.submeshes)):
				submesh = self.submeshes[submesh_id]
				if len(submesh.tris) > 0:
					faces_to_check.append((submesh_id, submesh.tris.pop()))
					faces_left -= 1
					break
				elif len(submesh.quads) > 0:
					faces_to_check.append((submesh_id, submesh.quads.pop()))
					faces_left -= 1
					break
			assert len(faces_to_check) > 0, 'Face not found!'
			# Continue as long as there are neighbors to check
			while len(faces_to_check) > 0:
				submesh_id_and_face = faces_to_check.pop()
				submesh_id = submesh_id_and_face[0]
				face = submesh_id_and_face[1]
				# Go vertices of face through and check which
				# of them are not handled yet.
				added_vrts = []
				for corner in range(0, len(face.vrts)):
					vrt_id = face.vrts[corner]
					if vrt_id not in vrt_map:
						new_vrt_id = len(new_vrts)
						vrt_map[vrt_id] = new_vrt_id
						face.vrts[corner] = new_vrt_id
						new_vrts.append(copy.deepcopy(self.vrts[vrt_id]))
						added_vrts.append(vrt_id)
					else:
						face.vrts[corner] = vrt_map[vrt_id]
				if len(face.vrts) == 3:
					new_submeshes[submesh_id].tris.append(face)
				else:
					assert len(face.vrts) == 4, 'Quad expected!'
					new_submeshes[submesh_id].quads.append(face)
				# Check which faces should be added to
				# unhandled. Select those that use recently
				# added vertices.
				for submesh_id in range(0, len(self.submeshes)):
					submesh = self.submeshes[submesh_id]
					tri_id = 0
					while tri_id < len(submesh.tris):
						face = submesh.tris[tri_id]
						add_this_face = False
						for added_vrt in added_vrts:
							if added_vrt in face.vrts:
								add_this_face = True
								break
						if add_this_face:
							faces_to_check.append((submesh_id, submesh.tris.pop(tri_id)))
							faces_left -= 1
						else:
							tri_id += 1
					quad_id = 0
					while quad_id < len(submesh.quads):
						face = submesh.quads[quad_id]
						add_this_face = False
						for added_vrt in added_vrts:
							if added_vrt in face.vrts:
								add_this_face = True
								break
						if add_this_face:
							faces_to_check.append((submesh_id, submesh.quads.pop(quad_id)))
							faces_left -= 1
						else:
							quad_id += 1

			# All connected faces have now been selected
			# and Mesh can be formed from them.
			new_mesh = Mesh()
			new_mesh.vgroups = copy.deepcopy(self.vgroups)
			new_mesh.name = copy.deepcopy(self.name)
			new_mesh.vrts = new_vrts
			new_mesh.submeshes = new_submeshes
			assert new_mesh.isClosed(), 'Loose mesh is not closed!'
			loose_meshes.append(new_mesh)

		# Finally set this Mesh as one of the loose meshes, and return
		# rest of them.
		if len(loose_meshes) > 0:
			new_self = loose_meshes.pop()
			self.vrts = new_self.vrts
			self.submeshes = new_self.submeshes
		return loose_meshes

	# If exists, returns one of face pairs, that make this Mesh concave.
	# Result is four sized list, where first are shared vertices of two
	# faces that form concave edge. Then are one vertices from both faces
	# that are not shared by those faces. If Mesh is convex, then None is
	# returned.
	# TODO: Support quads in future!
	def getConcavingFaces(self):
		edges = {}
		CONCAVE_DIFF = 0.01
		for submesh_id in range(0, len(self.submeshes)):
			submesh = self.submeshes[submesh_id]
			assert len(submesh.quads) == 0, 'Quads not supported yet!'
			for tri_id in range(0, len(submesh.tris)):
				face = submesh.tris[tri_id]
				for corner in range(0, len(face.vrts)):
					vrt0 = face.vrts[corner]
					vrt1 = face.vrts[(corner + 1) % len(face.vrts)]
					if (vrt0, vrt1) in edges:
						edge = vrt0, vrt1
					else:
						edge = vrt1, vrt0
					# Check if edge already exists
					if edge in edges:
						# This edge is already added.
						# It means, that there are some
						# faces that share this edge.
						# Test if this makes Mesh
						# concave.
						for face_ident in edges[edge]:
							f_submesh_id = face_ident[0]
							f_tri_id = face_ident[1]
							face2 = self.submeshes[f_submesh_id].tris[f_tri_id]
							result = [vrt0, vrt1]
							# Find vertices that are not shared
							for vrt in face.vrts:
								if vrt not in result:
									result.append(vrt)
									break
							for vrt in face2.vrts:
								if vrt not in result:
									result.append(vrt)
									break
							# Fix result to contain positions
							for offset in range(0, len(result)):
								result[offset] = self.vrts[result[offset]].pos
							# Now check if this edge makes mesh concave
							face_normal = (self.vrts[face.vrts[1]].pos - self.vrts[face.vrts[0]].pos).cross(self.vrts[face.vrts[2]].pos - self.vrts[face.vrts[0]].pos)
							face_normal.length = 1.0
							if hpp_misc.distanceToPlane(result[3], self.vrts[face.vrts[0]].pos, face_normal) > CONCAVE_DIFF:
								return result
					else:
						edges[edge] = []
					edges[edge].append((submesh_id, tri_id))
		# Nothing found. Mesh is convex.
		return None

	# Checks if list of vertex ID's contains only unique vertices
	def verticesUnique(self, verts):
		for verts_id1 in range(0, len(verts)):
			for verts_id2 in range(verts_id1 + 1, len(verts)):
				if verts[verts_id1] == verts[verts_id2]:
					return False
		return True

	# Cut Mesh in half using specific plane. Quads are not supported yet.
	# May raise MeshCutError in case floating point errors make impossible
	# states. If MeshCutError is raised, then Mesh is not modified.
	def cutWithPlane(self, plane_pos, plane_normal, plane_smooth = False, plane_tex_u_vec = None, plane_tex_v_vec = None, submesh_id = 0):

		assert self.isClosed()

		MIN_CUT_DEPTH = 0.0001

		plane_normal_normalized = mathutils.Vector(plane_normal)
		plane_normal_normalized.length = 1.0

		# TODO: Remove this when quads are supported!
		self.convertQuadsToTris(MeshFace.SHORTER)
		newvrts = []
		newsubmeshes = []
		cutlines = []
		# Cut original faces
		for submesh in self.submeshes:
			assert len(submesh.quads) == 0, 'Quads are not yet supported!'
			# Create new submesh
			newsubmesh = MeshSubmesh()
			newsubmesh.uvlayers_compcounts = copy.deepcopy(submesh.uvlayers_compcounts)
			newsubmesh.mat = copy.deepcopy(submesh.mat)
			# Go triangles through
			for tri in submesh.tris:
				# Check which vertices are on which side of plane
				vrts_front = []
				vrts = []
				uvs = []
				vrts_depth = []
				vrts_at_plane = []
				unsure_vrts = []
				unsures_to_front = True
				for corner in range(0, 3):
					vrt = self.vrts[tri.vrts[corner]]
					depth = -hpp_misc.distanceToPlane(vrt.pos, plane_pos, plane_normal_normalized)
					front = depth < 0.0
					if depth > -MIN_CUT_DEPTH and depth < MIN_CUT_DEPTH:
						unsure_vrts.append(corner)
						vrts_at_plane.append(True)
					else:
						unsures_to_front = front
						vrts_at_plane.append(False)
					vrts_front.append(front)
					vrts.append(vrt)
					uvs.append(tri.uvs[corner])
					vrts_depth.append(depth)
				for unsure_vrt in unsure_vrts:
					vrts_front[unsure_vrt] = unsures_to_front
					vrts_depth[unsure_vrt] = 0.0

				# First try easy cases where triangle is fully behind or front and special cases where one of vertices is at plane
				cutting_done = False
				for rotate in range(0, 3):

					# Case where triangle is 100% front of plane
					if vrts_front[0] and vrts_front[1] and vrts_front[2]:
						# Nothing needs to be done
						cutting_done = True
						break

					# First case where first vertex is on plane
					if vrts_at_plane[0] and vrts_front[1] and not vrts_front[2] and not vrts_at_plane[1] and not vrts_at_plane[2]:
						# Get cutpoints
						cp_and_uvs_12 = self.getCutpoint(plane_pos, plane_normal, vrts[1], vrts[2], uvs[1], uvs[2])
						cp12 = cp_and_uvs_12[0]
						uvs12 = cp_and_uvs_12[1]
						# Find vertices
						vrt0_id = self.findNearVertex(newvrts, vrts[0])
						vrt2_id = self.findNearVertex(newvrts, vrts[2])
						cp12_vrt_id = self.findNearVertex(newvrts, cp12)
						# Form face only if all vertices are different ones.
						if self.verticesUnique([vrt0_id, cp12_vrt_id, vrt2_id]):
							newtri = MeshFace(3)
							newtri.vrts = [vrt0_id, cp12_vrt_id, vrt2_id]
							newtri.uvs[0] = uvs[0]
							newtri.uvs[1] = uvs12
							newtri.uvs[2] = uvs[2]
							newtri.smooth = tri.smooth
							assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
							newsubmesh.tris.append(newtri)
							# Form cutlines
							cutlines.append([cp12_vrt_id, vrt0_id])
						cutting_done = True
						break

					# Second case where first vertex is on plane
					if vrts_at_plane[0] and not vrts_front[1] and vrts_front[2] and not vrts_at_plane[1] and not vrts_at_plane[2]:
						# Get cutpoints
						cp_and_uvs_12 = self.getCutpoint(plane_pos, plane_normal, vrts[1], vrts[2], uvs[1], uvs[2])
						cp12 = cp_and_uvs_12[0]
						uvs12 = cp_and_uvs_12[1]
						# Find vertices
						vrt0_id = self.findNearVertex(newvrts, vrts[0])
						vrt1_id = self.findNearVertex(newvrts, vrts[1])
						cp12_vrt_id = self.findNearVertex(newvrts, cp12)
						# Form face only if all vertices are different ones.
						if self.verticesUnique([vrt0_id, vrt1_id, cp12_vrt_id]):
							newtri = MeshFace(3)
							newtri.vrts = [vrt0_id, vrt1_id, cp12_vrt_id]
							newtri.uvs[0] = uvs[0]
							newtri.uvs[1] = uvs[1]
							newtri.uvs[2] = uvs12
							newtri.smooth = tri.smooth
							assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
							newsubmesh.tris.append(newtri)
							# Form cutlines
							cutlines.append([vrt0_id, cp12_vrt_id])
						cutting_done = True
						break

					# Case where triangle is 100% at back of plane
					if not vrts_front[0] and not vrts_front[1] and not vrts_front[2]:
						# Find vertices
						vrt0_id = self.findNearVertex(newvrts, vrts[0])
						vrt1_id = self.findNearVertex(newvrts, vrts[1])
						vrt2_id = self.findNearVertex(newvrts, vrts[2])
						# Form face
						newtri = MeshFace(3)
						newtri.vrts = [vrt0_id, vrt1_id, vrt2_id]
						newtri.uvs = uvs
						newtri.smooth = tri.smooth
						assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
						newsubmesh.tris.append(newtri)
						cutting_done = True
						break

					# Do "rotating", so that every situation becomes detected.
					vrts_front.append(vrts_front[0])
					vrts_front.pop(0)
					vrts.append(vrts[0])
					vrts.pop(0)
					uvs.append(uvs[0])
					uvs.pop(0)
					vrts_depth.append(vrts_depth[0])
					vrts_depth.pop(0)
					vrts_at_plane.append(vrts_at_plane[0])
					vrts_at_plane.pop(0)

				# Then try rest of cases
				while not cutting_done:

					assert not (vrts_at_plane[0] and vrts_front[1] and not vrts_front[2] and not vrts_at_plane[1] and not vrts_at_plane[2]), 'Should have been detected earlier!'
					assert not (vrts_at_plane[0] and not vrts_front[1] and vrts_front[2] and not vrts_at_plane[1] and not vrts_at_plane[2]), 'Should have been detected earlier!'

					# Case where only one vertex is at back of plane
					if not vrts_front[0] and vrts_front[1] and vrts_front[2]:
						# Get cutpoints
						cp_and_uvs_01 = self.getCutpoint(plane_pos, plane_normal, vrts[0], vrts[1], uvs[0], uvs[1])
						cp_and_uvs_02 = self.getCutpoint(plane_pos, plane_normal, vrts[0], vrts[2], uvs[0], uvs[2])
						cp01 = cp_and_uvs_01[0]
						uvs01 = cp_and_uvs_01[1]
						cp02 = cp_and_uvs_02[0]
						uvs02 = cp_and_uvs_02[1]
						# Find vertices
						vrt0_id = self.findNearVertex(newvrts, vrts[0])
						cp01_vrt_id = self.findNearVertex(newvrts, cp01)
						cp02_vrt_id = self.findNearVertex(newvrts, cp02)
						# Form face only if all vertices are different ones.
						if self.verticesUnique([vrt0_id, cp01_vrt_id, cp02_vrt_id]):
							newtri = MeshFace(3)
							newtri.vrts = [vrt0_id, cp01_vrt_id, cp02_vrt_id]
							newtri.uvs[0] = uvs[0]
							newtri.uvs[1] = uvs01
							newtri.uvs[2] = uvs02
							newtri.smooth = tri.smooth
							assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
							newsubmesh.tris.append(newtri)
							# Form cutlines
							cutlines.append([cp02_vrt_id, cp01_vrt_id])
						break

					# Case where two vertices are at back of plane
					if not vrts_front[0] and not vrts_front[1] and vrts_front[2]:
						# Get cutpoints
						cp_and_uvs_02 = self.getCutpoint(plane_pos, plane_normal, vrts[0], vrts[2], uvs[0], uvs[2])
						cp_and_uvs_12 = self.getCutpoint(plane_pos, plane_normal, vrts[1], vrts[2], uvs[1], uvs[2])
						cp02 = cp_and_uvs_02[0]
						uvs02 = cp_and_uvs_02[1]
						cp12 = cp_and_uvs_12[0]
						uvs12 = cp_and_uvs_12[1]
						# Find vertices
						vrt0_id = self.findNearVertex(newvrts, vrts[0])
						vrt1_id = self.findNearVertex(newvrts, vrts[1])
						cp02_vrt_id = self.findNearVertex(newvrts, cp02)
						cp12_vrt_id = self.findNearVertex(newvrts, cp12)
						# Form face only if all vertices are different ones.
						if self.verticesUnique([vrt0_id, vrt1_id, cp12_vrt_id]):
							newtri = MeshFace(3)
							newtri.vrts = [vrt0_id, vrt1_id, cp12_vrt_id]
							newtri.uvs[0] = uvs[0]
							newtri.uvs[1] = uvs[1]
							newtri.uvs[2] = uvs12
							newtri.smooth = tri.smooth
							assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
							newsubmesh.tris.append(newtri)
						# Form face only if all vertices are different ones.
						if self.verticesUnique([vrt0_id, cp12_vrt_id, cp02_vrt_id]):
							newtri = MeshFace(3)
							newtri.vrts = [vrt0_id, cp12_vrt_id, cp02_vrt_id]
							newtri.uvs[0] = uvs[0]
							newtri.uvs[1] = uvs12
							newtri.uvs[2] = uvs02
							newtri.smooth = tri.smooth
							assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
							newsubmesh.tris.append(newtri)
						# Form cutline only if all vertices are different ones.
						# TODO: This should be done differently. We can conclude if vertices of faces are unique, that this is too!
						if self.verticesUnique([cp02_vrt_id, cp12_vrt_id]):
							cutlines.append([cp02_vrt_id, cp12_vrt_id])
						break

					# Do "rotating", so that every situation becomes detected.
					vrts_front.append(vrts_front[0])
					vrts_front.pop(0)
					vrts.append(vrts[0])
					vrts.pop(0)
					uvs.append(uvs[0])
					uvs.pop(0)
					vrts_depth.append(vrts_depth[0])
					vrts_depth.pop(0)
					vrts_at_plane.append(vrts_at_plane[0])
					vrts_at_plane.pop(0)

			newsubmeshes.append(newsubmesh)

		cutlines_old = cutlines
		cutlines = []

		# Now go all faces through, and check which faces does not have
		# a counterpart for their edge. Cutlines are formed from these.
		edges = {}
		for submesh in newsubmeshes:
			for tri in submesh.tris:
				for corner in range(0, 3):
					vrt0 = tri.vrts[corner]
					vrt1 = tri.vrts[(corner + 1) % 3]
					assert vrt0 != vrt1, 'Found face that has same vertex two times!'
					edge = vrt1, vrt0
					if edge not in edges:
						edges[edge] = 1
					else:
						edges[edge] += 1
			for quad in submesh.quads:
				for corner in range(0, 4):
					vrt0 = quad.vrts[corner]
					vrt1 = quad.vrts[(corner + 1) % 4]
					edge = vrt1, vrt0
					if edge not in edges:
						edges[edge] = 1
					else:
						edges[edge] += 1
		# Now all edges have been added. Now check which
		# one of them does not have a counterpart.
		while len(edges) > 0:
			edge_and_count = edges.popitem();
			edge = edge_and_count[0]
			count = edge_and_count[1]
			counter_edge = edge[1], edge[0]
			if counter_edge in edges:
				counter_count = edges[counter_edge]
				del edges[counter_edge]
				if count - counter_count == 0:
					pass
				elif count - counter_count == 1:
					cutlines.append([edge[0], edge[1]])
				elif count - counter_count == -1:
					cutlines.append([edge[1], edge[0]])
				else:
					assert False, 'Counter edge has too big or small count! Their difference is ' + str(count - counter_count) + '. Only -1, 0 and 1 are allowed!'
			else:
				assert count == 1, 'No counter edge found for ' + str(count) + ' identical edges! This is only allowed for one edge!'
				cutlines.append([edge[0], edge[1]])

		# Original faces are now cut. Now it is time to fill holes that
		# are bordered by cutlines. Cutlines consist of two sized lists,
		# that tell from which vertex to which the cutline goes. So
		# there should be follower for all cutline and this way they
		# should form paths, if cut has been successfull. That is, if
		# no floatin point errors have ruined it. Next, these paths
		# will be formed. These paths will be separated to holes and
		# islands. Islands are holes that are "inside out", meaning
		# they are "islands" inside holes.
		holes = []
		islands = []
		while len(cutlines) > 0:
			# Pick last cutline, and form border around this hole
			cutline = cutlines.pop()
			begin = cutline[0]
			newcutpath = []
			newcutpath.append(begin)
			while (cutline[1] != begin):
				# Find second cutline of this path
				cutline_id = -1
				for cutline_id2 in range(0, len(cutlines)):
					if cutlines[cutline_id2][0] == cutline[1]:
						cutline_id = cutline_id2
						break
				# Ensure cutline was found
				if cutline_id < 0:
					raise MeshCutError('No following cutline found!')
				cutline = cutlines.pop(cutline_id)
				# Ensure no doubles are made
				vrt_id = cutline[0]
				if len(newcutpath) == 0 or newcutpath[-1] != vrt_id:
					newcutpath.append(vrt_id)
			# Ensure end and begin vertices are not same
			while len(newcutpath) > 0 and newcutpath[0] == newcutpath[-1]:
				newcutpath.pop()
			# If hole is too short, then skip this
			if len(newcutpath) == 0:
				continue
			# Ensure hole is big enough
			if len(newcutpath) < 3:
				raise MeshCutError('Cutpath has too small amount of vertices!')
			# Travel cutpath to see if it forms turn of 360°
			# negative or positive. Positive means it is hole and
			# negative an island.
			turn = 0.0
			for travel in range(0, len(newcutpath)):
				prev_id = newcutpath[(travel + len(newcutpath) - 1) % len(newcutpath)]
				this_id = newcutpath[travel]
				next_id = newcutpath[(travel + 1) % len(newcutpath)]
				to_prev = newvrts[prev_id].pos - newvrts[this_id].pos
				to_next = newvrts[next_id].pos - newvrts[this_id].pos
				assert to_prev.length > 0.0 and to_next.length > 0, 'Zero length vectors not accepted here!'
				diffangle = 180.0 - mathutils.AngleBetweenVecs(to_prev, to_next)
				if plane_normal.dot(to_next.cross(to_prev)) > 0:
					turn += diffangle
				else:
					turn -= diffangle
			if abs(abs(turn) - 360.0) > 2.5:
				raise MeshCutError('Cutpath has invalid travel angle (' + str(turn) + ' degrees)!')
			if turn > 0.0:
				holes.append(newcutpath)
			else:
				islands.append(newcutpath)

		# Now we need to get rid of islands. This is done by adding
		# double cutline between island and hole. That way island is
		# joined to hole, and they became new hole. We need to be sure,
		# that the joining double cutline will not hit to any other
		# cutlines. We also need to ensure the cutline is at the area
		# of hole.
		while len(islands) > 0:
			# Pick first island
			island_id = len(islands) - 1
			island = islands[island_id]

			assert len(holes) > 0, 'No holes!'

			# Go all points of this island and all points of all
			# holes through, and check if there is a pair that can
			# be connected with line, without colliding to other
			# lines and without making the line outside island or
			# hole.
			island_len = len(island)
			nearest_ipoint_id = -1
			nearest_hole_id = -1
			nearest_hpoint_id = -1
			nearest = 9999999.9 # If negative, then use same point!
			for ipoint_id in range(0, island_len):
				i_this = newvrts[island[ipoint_id]].pos
				i_next = newvrts[island[(ipoint_id + 1) % island_len]].pos
				i_prev = newvrts[island[(ipoint_id + island_len - 1) % island_len]].pos
				i_to_next = i_next - i_this
				i_to_prev = i_prev - i_this
				i_max_angle = self.angleAtPlane(i_to_next, i_to_prev, plane_normal)
				for hole_id in range(0, len(holes)):
					hole = holes[hole_id]
					hole_len = len(hole)
					for hpoint_id in range(0, hole_len):

						# Special case! If hole and island have same vertex,
						# then they can be merged immediately without line,
						# but by using just this single vertex.
						if island[ipoint_id] == hole[hpoint_id]:
							nearest = -1.0
							nearest_ipoint_id = ipoint_id
							nearest_hole_id = hole_id
							nearest_hpoint_id = hpoint_id
							break

						h_this = newvrts[hole[hpoint_id]].pos
						h_next = newvrts[hole[(hpoint_id + 1) % hole_len]].pos
						h_prev = newvrts[hole[(hpoint_id + hole_len - 1) % hole_len]].pos

						# Form the line (from island towards hole)
						line = h_this - i_this
						line_length = line.length
						# If this line is longer than already found, then give up
						if line_length > nearest:
							continue
						# Ensure line does not go through island.
						angle_to_next = self.angleAtPlane(i_to_next, line, plane_normal)
						angle_to_prev = self.angleAtPlane(line, i_to_prev, plane_normal)
						if angle_to_next > i_max_angle or angle_to_prev > i_max_angle:
							continue

						# Ensure line does not hit hole
						h_to_next = h_next - h_this
						h_to_prev = h_prev - h_this
						h_max_angle = self.angleAtPlane(h_to_next, h_to_prev, plane_normal)
						angle_to_next = self.angleAtPlane(h_to_next, -line, plane_normal)
						angle_to_prev = self.angleAtPlane(-line, h_to_prev, plane_normal)
						if angle_to_next > h_max_angle or angle_to_prev > h_max_angle:
							continue
						# Now ensure line will not hit any other lines
						if self.hitsOtherLines(newvrts, i_this, h_this, holes, hole_id, hpoint_id):
							continue
						if self.hitsOtherLines(newvrts, i_this, h_this, islands, island_id, ipoint_id):
							continue
						# Join can be done here!
						nearest = line_length
						nearest_ipoint_id = ipoint_id
						nearest_hole_id = hole_id
						nearest_hpoint_id = hpoint_id
					if nearest < 0.0:
						break
				if nearest < 0.0:
					break

			if nearest_ipoint_id >= 0:
				# Do merge
				if nearest >= 0.0:
					hole = holes[nearest_hole_id]
					newhole = hole[0:nearest_hpoint_id+1] + island[nearest_ipoint_id:] + island[:nearest_ipoint_id+1] + hole[nearest_hpoint_id:]
					holes[nearest_hole_id] = newhole
				# Special case
				else:
					hole = holes[nearest_hole_id]
					hole = hole[0:nearest_hpoint_id] + island[nearest_ipoint_id:] + island[0:nearest_ipoint_id] + hole[nearest_hpoint_id:]
					holes[nearest_hole_id] = hole

				# This island is merged, so it can be removed.
				islands.pop(island_id)
			else:
				# Island could not be merged, so add it to
				# front of list to wait for new opportunity.
				island = islands.pop(island_id)
				islands.insert(0, island)

		# Now there are only holes. Close them one by one.
		while len(holes) > 0:
			hole = holes.pop()

			# Closing is done by finding a vertex that has
			# neighbourvertices that form triangle that "opens"
			# inside to the hole. That way hole is closed little
			# bit a time.
			while len(hole) >= 3:

				# Before removing points, it is good idea to
				# check if there are any infinite thin dead
				# ends. This means situation X, Y, X. There are
				# same vertices and some vertex is between
				# them. Also, sometimes X, X situations occure.
				# TODO: Find out why X, X situations occure!
				self.removeThinDeadEndsAndDoubles(hole)

				hole_len = len(hole)
				# For best result, it is good idea to add the
				# new face to there, where it has least same
				# angle with it's neighbors.
				best_point = -1
				best_point_angle = 999.9
				# A little extra to select better faces. If no faces are found, then this value is slowly decreased in hope of finding faces then.
				minangle_add = 15
				while minangle_add >= 0 and best_point < 0:
					for point_id in range(0, hole_len):
						next_point_id = (point_id + 1) % hole_len
						prev_point_id = (point_id + hole_len - 1) % hole_len
						this = newvrts[hole[point_id]].pos
						next = newvrts[hole[next_point_id]].pos
						prev = newvrts[hole[prev_point_id]].pos
						to_next = next - this
						to_prev = prev - this
						# Ensure, that if edge would be added
						# between prev and next, it would not
						# leave any vertices outside of hole.
						newedge = next - prev
						to_next_minangle = self.angleAtPlane(-newedge, -to_next, plane_normal)
						to_prev_minangle = self.angleAtPlane(-to_prev, newedge, plane_normal)
						# Increase min angle a little bit to avoid making
						# edge that would be really near to other vertices
						to_next_minangle += minangle_add
						to_prev_minangle += minangle_add
						hits = False
						for checkpoint_id in range(0, hole_len):
							# Skip vertices of possible new triangle.
							if hole[checkpoint_id] == hole[point_id] or hole[checkpoint_id] == hole[next_point_id] or hole[checkpoint_id] == hole[prev_point_id]:
								continue
							checkpoint_pos = newvrts[hole[checkpoint_id]].pos
							next_to_cp = checkpoint_pos - next
							if next_to_cp.length == 0.0:
								continue
							to_next_angle = self.angleAtPlane(next_to_cp, -to_next, plane_normal)
							if to_next_angle >= to_next_minangle:
								continue
							prev_to_cp = checkpoint_pos - prev
							if prev_to_cp.length == 0.0:
								continue
							to_prev_angle = self.angleAtPlane(-to_prev, prev_to_cp, plane_normal)
							if to_prev_angle >= to_prev_minangle:
								continue
							hits = True
							break
						if hits:
							continue
						# Ensure no corners are allowed, that
						# has bigger than 180° angle.
						this_angle = self.angleAtPlane(to_next, to_prev, plane_normal)
						if this_angle >= 180.0:
							continue
						if this_angle < best_point_angle:
							best_point = point_id
							best_point_angle = this_angle
					minangle_add -= 1
				assert best_point >= 0
				vrt0 = hole[(best_point + hole_len - 1) % hole_len]
				vrt1 = hole[best_point]
				vrt2 = hole[(best_point + 1) % hole_len]
				try:
					newsubmeshes[submesh_id].tris.append(self.formTriToPlane(newvrts, [vrt0, vrt1, vrt2], plane_pos, plane_normal, plane_smooth, plane_tex_u_vec, plane_tex_v_vec))
				except:
					raise MeshCutError('Too hard hole to fill!')
				if len(newsubmeshes[submesh_id].tris) > 0:
					uv_layers = len(newsubmeshes[submesh_id].tris[0].uvs[0])
					for tri2 in newsubmeshes[submesh_id].tris:
						assert uv_layers == len(tri2.uvs[0])
						assert uv_layers == len(tri2.uvs[1])
						assert uv_layers == len(tri2.uvs[2])
				hole.pop(best_point)

		# Before declaring this as completed, ensure it is closed
		mesh_test = copy.deepcopy(self)
		mesh_test.vrts = copy.deepcopy(newvrts)
		mesh_test.submeshes = copy.deepcopy(newsubmeshes)
		if not mesh_test.isClosed():
			raise MeshCutError('Resulting mesh is not closed!')

		# Cutting is completed! Replace old data with new one!
		self.vrts = newvrts
		self.submeshes = newsubmeshes

		# Finish cutting by optimizing useless vertices away
		vrts_backup = copy.deepcopy(self.vrts)
		submeshes_backup = copy.deepcopy(self.submeshes)
		try:
			self.mergeUselessVertices()
		except:
			self.vrts = copy.deepcopy(vrts_backup)
			self.submeshes = copy.deepcopy(submeshes_backup)
			raise

		assert self.isClosed(), 'Not closed after optimization!'

	# Checks if mesh is closed
	def isClosed(self):
		edges = {}
		MERGE_VERTICES_DISTANCE = 0.00001
		for submesh in self.submeshes:
			for tri in submesh.tris:
				for corner in range(0, 3):
					vrt0 = tri.vrts[corner]
					vrt1 = tri.vrts[(corner + 1) % 3]
					edge = vrt0, vrt1
					if edge not in edges:
						edges[edge] = 1
					else:
						edges[edge] += 1
			for quad in submesh.quads:
				for corner in range(0, 4):
					vrt0 = quad.vrts[corner]
					vrt1 = quad.vrts[(corner + 1) % 4]
					edge = vrt0, vrt1
					if edge not in edges:
						edges[edge] = 1
					else:
						edges[edge] += 1
		# Now all edges have been added. Now ensure there are
		# counterparts for every edge.
		while len(edges) > 0:
			edge = edges.popitem();
			counter_edge = edge[0][1], edge[0][0]
			if counter_edge not in edges or edges[counter_edge] != edge[1]:
				# Before giving up, check if this edge could be
				# merged, so the problem would perish.
				vrt1 = edge[0][0]
				vrt2 = edge[0][1]
				pos1 = self.vrts[vrt1].pos
				pos2 = self.vrts[vrt2].pos
				edge_len = (pos1 - pos2).length
				if edge_len <= MERGE_VERTICES_DISTANCE:
					center = (pos1 + pos2) / 2.0
					self.mergeVertices(vrt1, vrt2, center)
					return self.isClosed()
				print('Edge ' + str(self.vrts[edge[0][0]].pos) + ' --- ' + str(self.vrts[edge[0][1]].pos) + ' is open or has invalid number of faces connected to it!')
				return False
			del edges[counter_edge]
		return True

	# Merges two vertices into one to specific position. Removes faces
	# whose area becomes zero.
	# TODO: Code support for quads!
	def mergeVertices(self, v1_id, v2_id, pos):
		# First handle vertices
		# TODO: Handle VGroups too!
		if v1_id != v2_id:
			self.vrts[v1_id].pos = pos
			self.vrts.pop(v2_id)
		# Now fix submeshes
		for submesh in self.submeshes:
			new_tris = []
			assert len(submesh.quads) == 0, 'Quads not supported yet!'
			for tri in submesh.tris:
				# Skip those that has become zero area sized.
				if v1_id in tri.vrts and v2_id in tri.vrts:
					continue
				if v1_id != v2_id:
					for corner in range(0, len(tri.vrts)):
						if tri.vrts[corner] == v2_id:
							tri.vrts[corner] = v1_id
						if tri.vrts[corner] > v2_id:
							tri.vrts[corner] -= 1
				new_tris.append(tri)
			submesh.tris = new_tris

	def spawn(self):
		# Mesh
		# TODO: Fix to work in new Blender!
		bmesh = Blender.Mesh.New(self.name)

		# Vertices
		vrt_map = {}
		for vrt_id in range(0, len(self.vrts)):
			vrt = self.vrts[vrt_id]
			bmesh.vertices.extend(vrt.pos)
			bvrt_id = len(bmesh.vertices) - 1
			vrt_map[vrt_id] = bvrt_id
			# TODO: Add vertexgroup weights too!

		# Faces
		for submesh in self.submeshes:
			for face in (submesh.tris + submesh.quads):
				vrts = []
				for vrt in face.vrts:
					vrts.append(vrt_map[vrt])
				bmesh.faces.extend(vrts)

		# Object
		# TODO: Fix to work in new Blender!
		bobj = Blender.Object.New('Mesh', self.name)
		bobj.link(bmesh)
		# TODO: Fix to work in new Blender!
		Blender.Scene.GetCurrent().link(bobj)

	def serialize(self):
		result = bytes()

		# Name
		result += hpp_misc.stringToBytes(self.name)

		# Vertices
		result += hpp_misc.uInt32ToBytes(len(self.vrts))
		for vrt in self.vrts:
			result += vrt.serialize()

		# Vertexgroups
		result += hpp_misc.uInt32ToBytes(len(self.vgroups))
		for vgroup in self.vgroups:
			result += hpp_misc.stringToBytes(vgroup)

		# Submeshes
		result += hpp_misc.uInt32ToBytes(len(self.submeshes))
		for submesh in self.submeshes:
			result += submesh.serialize()

		return result

	def read(self, ifile):
		self.name = ''
		self.vrts = []
		self.vgroups = []
		self.submeshes = []

		# Name
		name_size = hpp_misc.readUInt32(ifile)
		self.name = ifile.read(name_size)

		# Vertices
		vrts_size = hpp_misc.readUInt32(ifile)
		while len(self.vrts) < vrts_size:
			new_vrt = MeshVertex()
			new_vrt.read(ifile)
			self.vrts.append(new_vrt)

		# Vertexgroups
		vgroups_size = hpp_misc.readUInt32(ifile)
		while len(self.vgroups) < vgroups_size:
			vgroup_size = readUInt32(ifile)
			vgroup = ifile.read(vgroup_size)
			self.vgroups.append(vgroup)

		# Submeshes
		submeshes_size = hpp_misc.readUInt32(ifile)
		while len(self.submeshes) < submeshes_size:
			new_submesh = MeshSubmesh()
			new_submesh.read(ifile)
			self.submeshes.append(new_submesh)

	# Internal function to find near vertex from list. If no vertex is
	# found, then new one is created. Returns ID of vertex.
	def findNearVertex(self, vrts, vrt):
		MAX_ALLOWED_DIFF = 0.00001
		MAX_ALLOWED_DIFF_TO_2 = MAX_ALLOWED_DIFF * MAX_ALLOWED_DIFF
		nearest_vrt_id = -1
		nearest_diff_to_2 = MAX_ALLOWED_DIFF_TO_2 * 2
		for vrt2_id in range(0, len(vrts)):
			vrt2 = vrts[vrt2_id]
			# Vertexgroups
			# If num of vgroups differ, then skip this one
			if len(vrt.vgroups) != len(vrt2.vgroups):
				break
			diff_to_2 = 0;
			vgroups_dont_match = False
			for vgroup_id, weight in vrt.vgroups.items():
				if vgroup_id not in vrt2.vgroups:
					vgroups_dont_match = True
					break
				subdiff = weight - vrt2.vgroups[vgroup_id]
				subdiff *= subdiff
				diff_to_2 += subdiff
			if vgroups_dont_match:
				break
			# Position
			for comp in range(0, 3):
				subdiff = vrt.pos[comp] - vrt2.pos[comp]
				subdiff *= subdiff
				diff_to_2 += subdiff
			# Calculate total diff
			if diff_to_2 <= MAX_ALLOWED_DIFF_TO_2 and diff_to_2 < nearest_diff_to_2:
				nearest_diff_to_2 = diff_to_2
				nearest_vrt_id = vrt2_id
		# Check if vertex was found
		if nearest_vrt_id >= 0:
			return nearest_vrt_id
		else:
			vrts.append(vrt)
			return len(vrts) - 1

	# Internal function to get cutpoints between vertices. Returns two
	# sized array that contains MeshVertex and UV layers from between them
	# that is at the plane
	def getCutpoint(self, plane_pos, plane_normal, vrt1, vrt2, uvs1, uvs2):
		ERROR_TOLERANCE_WEIGHT = 0.00001
		ERROR_TOLERANCE_DEPTH = 0.001
		assert len(uvs1) == len(uvs2), 'UV layers do not match!'
		v1_to_v2 = vrt2.pos - vrt1.pos
		dp_v1tov2_pn = v1_to_v2.dot(plane_normal)
		assert dp_v1tov2_pn != 0.0, 'getCutpoint: Divizion by zero!'
		vrt2_weight = (plane_normal.dot(plane_pos) - plane_normal.dot(vrt1.pos)) / dp_v1tov2_pn
		# Do some error tolerance
		if vrt2_weight < 0.0 and vrt2_weight >= -ERROR_TOLERANCE_WEIGHT: vrt2_weight = 0.0
		if vrt2_weight > 1.0 and vrt2_weight <= 1.0 + ERROR_TOLERANCE_WEIGHT: vrt2_weight = 1.0
		# If result is not valid, then try to calculate it through vertex #2
		if vrt2_weight < 0.0 or vrt2_weight > 1.0:
			vrt2_weight = 1.0 - (plane_normal.dot(plane_pos) - plane_normal.dot(vrt2.pos)) / dp_v1tov2_pn
		if vrt2_weight < 0.0 and vrt2_weight >= -ERROR_TOLERANCE_WEIGHT: vrt2_weight = 0.0
		if vrt2_weight > 1.0 and vrt2_weight <= 1.0 + ERROR_TOLERANCE_WEIGHT: vrt2_weight = 1.0
		# If result is still not valid, then final try is to check if another of vertices is near enough at cut plane
		if vrt2_weight < 0.0 or vrt2_weight > 1.0:
			assert plane_normal.length != 0.0, 'Normal is zero lengthed!'
			plane_normal_normalized = plane_normal / plane_normal.length
			vrt1_dst = hpp_misc.distanceToPlane(vrt1.pos, plane_pos, plane_normal_normalized)
			if vrt1_dst >= -ERROR_TOLERANCE_DEPTH and vrt1_dst <= ERROR_TOLERANCE_DEPTH:
				v2_weight = 0.0
			else:
				vrt2_dst = hpp_misc.distanceToPlane(vrt2.pos, plane_pos, plane_normal_normalized)
				if vrt2_dst >= -ERROR_TOLERANCE_DEPTH and vrt2_dst <= ERROR_TOLERANCE_DEPTH:
					v2_weight = 1.0
		assert vrt2_weight >= 0.0 and vrt2_weight <= 1.0, 'Line does not hit plane!'
		vrt1_weight = 1.0 - vrt2_weight
		cp = MeshVertex()
		# Pos
		cp.pos = vrt1.pos * vrt1_weight + vrt2.pos * vrt2_weight
		# Vertexgroups
		for vgroup_id, weight in vrt1.vgroups.items():
			cp.vgroups[vgroup_id] = weight * vrt1_weight
		for vgroup_id, weight in vrt2.vgroups.items():
			if vgroup_id not in cp.vroups:
				cp.vgroups[vgroup_id] = weight * vrt2_weight
			else:
				cp.vgroups[vgroup_id] += weight * vrt2_weight
		# UVs
		uvs = []
		for layer_id in range(0, len(uvs1)):
			uv1 = uvs1[layer_id]
			uv2 = uvs2[layer_id]
			assert len(uv1) == len(uv2), 'UV component counts do not match at layer #' + str(layer_id) + '!'
			uvs.append(uv1 * vrt1_weight + uv2 * vrt2_weight)
		return [cp, uvs]

	# Internal function. Returns angle at plane.
	def angleAtPlane(self, v1, v2, plane_normal):
		assert v1.length > 0.0, 'angleAtPlane(): Vector #1 has zero length!'
		assert v2.length > 0.0, 'angleAtPlane(): Vector #2 has zero length!'
		raw_angle = mathutils.AngleBetweenVecs(v1, v2);
		if plane_normal.dot(v1.cross(v2)) < 0:
			return 360.0 - raw_angle
		else:
			return raw_angle

	# Internal function to check if line hits other lines at plane
	def hitsOtherLines(self, vrts, v1, v2, cutpaths, cutpath_ignore_id, point_ignore_id):
		for cutpath_id in range(0, len(cutpaths)):
			cutpath = cutpaths[cutpath_id]
			cutpath_len = len(cutpath)
			for point_id in range(0, cutpath_len):

				# Check if this line should be ignored
				if cutpath_id == cutpath_ignore_id:
					if point_id == point_ignore_id:
						continue
					if (point_id + 1) % cutpath_len == point_ignore_id:
						continue

				cp1 = vrts[cutpath[point_id]].pos
				cp2 = vrts[cutpath[(point_id + 1) % cutpath_len]].pos

				if hpp_misc.linesegmentsIntersectAtPlane(cp1, cp2, v1, v2):
					return True
		return False

	# Internal function to add triangle to plane. The speciality of this
	# function is its ability to select correct uv values.
	def formTriToPlane(self, vrts, vrt_ids, plane_pos, plane_normal, plane_smooth, plane_tex_u_vec, plane_tex_v_vec):
		newtri = MeshFace(3)
		assert len(vrt_ids) == 3, 'Invalid number of vertex IDs!'
		newtri.vrts = vrt_ids
		newtri.smooth = plane_smooth
		for corner in range(0, 3):
			pos = vrts[vrt_ids[corner]].pos
			uv = hpp_misc.transformPointToTrianglespace(pos, plane_tex_u_vec, plane_tex_v_vec)
			newtri.uvs[corner].append(uv)
		assert len(newtri.vrts) == len(newtri.uvs), 'Vertex and UV counts differ!'
		assert (vrts[vrt_ids[1]].pos - vrts[vrt_ids[0]].pos).cross(vrts[vrt_ids[2]].pos - vrts[vrt_ids[0]].pos).dot(plane_normal) > 0.0, 'Resulting triangle is facing wrong way!'
		return newtri

	# Internal function
	def ensureUniqueVertices(self, cutpaths):
		cutpath_id = 0
		while cutpath_id < len(cutpaths):
			cutpath = cutpaths[cutpath_id]
			met_vrts = {}
			offset = 0
			while offset < len(cutpath):
				vrt = cutpath[offset]
				if vrt in met_vrts:
					# This vertex is already met, so we can
					# separate everything between these two
					# met points.
					new_cutpath = cutpath[met_vrts[vrt]:offset]
					# Remove new_cutpath-part from this cutpath
					cutpath = cutpath[0:met_vrts[vrt]] + cutpath[offset:]
					for fix in new_cutpath:
						del met_vrts[fix]
					offset -= len(new_cutpath)
					cutpaths.append(new_cutpath)
				assert vrt not in met_vrts
				met_vrts[vrt] = offset
				offset += 1
			cutpath_id += 1

	# Internal function
	def removeThinDeadEndsAndDoubles(self, cutpath):
		if len(cutpath) > 3:
			checkpoint_id = 0
			while checkpoint_id < len(cutpath):
				cutpath_len = len(cutpath)
				prev = cutpath[(checkpoint_id + cutpath_len - 1) % cutpath_len]
				this = cutpath[checkpoint_id]
				next = cutpath[(checkpoint_id + 1) % cutpath_len]
				if prev == this:
					cutpath.pop(checkpoint_id)
					if checkpoint_id == len(cutpath):
						checkpoint_id -= 1
				elif prev == next:
					cutpath.pop(checkpoint_id)
					if checkpoint_id == len(cutpath):
						checkpoint_id -= 1
				else: checkpoint_id += 1					

class ArmatureBone:
	def __init__(self, bone):
		self.pos = bone.head['ARMATURESPACE']
		assert len(self.pos) == 3
		self.rotmatrix = bone.matrix['BONESPACE']
		assert len(self.rotmatrix) == 3
		if bone.parent:
			self.parent = bone.parent.name
		else:
			self.parent = ''

	def serialize(self):
		result = bytes()
		result += hpp_misc.vectorToBytes(self.pos)
		result += hpp_misc.matrixToBytes(self.rotmatrix)
		result += hpp_misc.stringToBytes(self.parent)
		return result

class ArmatureIpoPoint:
	def __init__(self, point):
		# TODO: Convert time using FPS of animation!
		self.time = point.vec[1][0]
		self.value = point.vec[1][1]
		# TODO: Get weight!
		self.weight = 1.0

	def serialize(self):
		result = bytes()
		result += hpp_misc.floatToBytes(self.time)
		result += hpp_misc.floatToBytes(self.value)
		result += hpp_misc.floatToBytes(self.weight)
		return result

class ArmatureIpo:
	def __init__(self, ipo):
		self.channels = { 'LocX': [], 'LocY': [], 'LocZ': [], 'QuatX': [], 'QuatY': [], 'QuatZ': [], 'QuatW': [], 'ScaleX': [], 'ScaleY': [], 'ScaleZ': [] }
		curves = ipo.getCurves()
		for curve in curves:
			points = curve.getPoints()
			for point in points:
				new_point = ArmatureIpoPoint(point)
				self.channels[curve.getName()].append(new_point)

	def serializeChannel(self, channel_name):
		channel = self.channels[channel_name]
		result = bytes()
		result += hpp_misc.uInt32ToBytes(len(channel))
		for point in channel:
			result += point.serialize()
		return result

	def serialize(self):
		result = bytes()
		result += self.serializeChannel('LocX')
		result += self.serializeChannel('LocY')
		result += self.serializeChannel('LocZ')
		result += self.serializeChannel('QuatX')
		result += self.serializeChannel('QuatY')
		result += self.serializeChannel('QuatZ')
		result += self.serializeChannel('QuatW')
		result += self.serializeChannel('ScaleX')
		result += self.serializeChannel('ScaleY')
		result += self.serializeChannel('ScaleZ')
		return result

class ArmatureAction:
	def __init__(self, ipos):
		self.ipos = ipos

	def serialize(self):
		result = bytes()
		result += hpp_misc.uInt32ToBytes(len(self.ipos))
		for bone_name, ipo in self.ipos.items():
			result += hpp_misc.stringToBytes(bone_name)
			result += ipo.serialize()
		return result

class Armature:
	def __init__(self, armature):

		# Bones
		self.bones = {}
		for bone_name, bone in armature.bones.items():
			self.bones[bone_name] = ArmatureBone(bone)

		# Actions
		self.actions = {}
		# TODO: Fix to work in new Blender!
		all_acts = Blender.Armature.NLA.GetActions()
		for act_name, act in all_acts.items():

			valid_action = True

			ipos = act.getAllChannelIpos()
			new_ipos = {}
			for bone_name, ipo in ipos.items():
				# If this IPO does not affect to any bone of
				# armature, then this action is not for this
				# armature.
				if bone_name not in self.bones:
					valid_action = False
					break
				new_ipo = ArmatureIpo(ipo)
				new_ipos[bone_name] = new_ipo
			
			if valid_action:
				self.actions[act_name] = ArmatureAction(new_ipos)

	def serialize(self):
		result = bytes()

		result += hpp_misc.uInt32ToBytes(len(self.bones))
		for bone_name, bone in self.bones.items():
			result += hpp_misc.stringToBytes(bone_name)
			result += bone.serialize()
		
		result += hpp_misc.uInt32ToBytes(len(self.actions))
		for act_name, act in self.actions.items():
			result += hpp_misc.stringToBytes(act_name)
			result += act.serialize()
		
		return result

class Material:
	def __init__(self, mat):

		# Name
		self.name = mat.name

		# Color
		color_raw = mat.getRGBCol()
		self.color = mathutils.Vector([color_raw[0], color_raw[1], color_raw[2]])

		# Specular color and shininess
		color_raw = mat.getSpecCol()
		self.specular = mathutils.Vector([color_raw[0], color_raw[1], color_raw[2]]) * mat.getSpec()
		self.shininess = mat.getHardness()

		# Ambient light multiplier
		self.ambient = mat.getAmb()

		# Emitting multiplier
		self.emit = mat.getEmit()

		# Alpha multiplier
		self.alpha = mat.getAlpha()

	def serialize(self):
		result = bytes()

		result += hpp_misc.stringToBytes(self.name)

		result += hpp_misc.vectorToBytes(self.color)

		result += hpp_misc.vectorToBytes(self.specular)
		result += hpp_misc.floatToBytes(self.shininess)

		result += hpp_misc.floatToBytes(self.ambient)

		result += hpp_misc.floatToBytes(self.emit)

		result += hpp_misc.floatToBytes(self.alpha)

		return result

class Object:
	def __init__(self, obj):

		if obj.getType() != 'Mesh':
			raise Exception('Unable to create Object from other types than Meshes!')

		self.transf = obj.getMatrix()
		self.mesh = obj.data.name

	def serialize(self):
		result = bytes()

		# Transformation
		result += hpp_misc.matrixToBytes(self.transf)

		# Mesh
		result += hpp_misc.stringToBytes(self.mesh)

		return result

class Room:
	def __init__(self, obj):

		if obj.getType() != 'Mesh':
			raise Exception('Unable to create Room because it is not Mesh!')

		self.transf = obj.getMatrix()
		self.mesh = obj.data.name

	def serialize(self):
		result = bytes()

		# Transformation
		result += hpp_misc.matrixToBytes(self.transf)

		# Mesh
		result += hpp_misc.stringToBytes(self.mesh)

		return result

class Portal:
	def __init__(self, obj):

		MAX_NORMAL_DIFF = 0.1

		obj_name = obj.getName()

		if obj.getType() != 'Mesh':
			raise Exception('Portal \"' + obj_name + '\" has invalid type! It should be Mesh!')

		# Names of rooms
		# Remove possible .XXX from end, where XXX is number
		dot_pos = obj_name.find('.', 6)
		if dot_pos != -1:
			for check_offset in range(dot_pos+1, len(obj_name)):
				asc_val = ord(obj_name[check_offset])
				if asc_val < ord('0') or asc_val > ord('9'):
					raise Exception('Portal \"' + obj_name + '\" has invalid name! Only numbers are allowed after the dot!')
			obj_name = obj_name[0:dot_pos]
		underscore_pos = obj_name.find('_', 6)
		if underscore_pos == -1:
			raise Exception('Found portal that does not have proper name! Portals should be named \"PortalXXX_YYY\" where XXX is the room where normal of portal points and YYY is the room at the other side.')
		self.frontroom_name = obj_name[6:underscore_pos]
		self.backroom_name = obj_name[underscore_pos+1:]

		# Vertices
		# Get position of portal
		obj_m = obj.getMatrix()
		mesh = obj.data
		self.pos = mathutils.Vector([0, 0, 0])
		if len(mesh.vertices) == 0:
			raise Exception('Portal \"' + obj_name + '\" has no vertices!')
		for vert in mesh.vertices:
			pos_abs = vert.co * obj_m
			self.pos += pos_abs
		self.pos /= len(mesh.vertices)
		# Get normal of portal and ensure rest of faces have same normal
		if len(mesh.faces) == 0:
			raise Exception('Portal \"' + obj_name + '\" has no faces! At least one is needed to define the front side of portal!')
		self.normal = mesh.faces[0].no * obj_m.rotationPart()
		for face in mesh.faces:
			normal = face.no * obj_m.rotationPart()
			angle = mathutils.AngleBetweenVecs(self.normal, normal)
			if angle > MAX_NORMAL_DIFF:
				raise Exception('Portal \"' + obj_name + '\" has faces with different normal! Maximum difference between normals is ' + str(MAX_NORMAL_DIFF) + ' degrees!')
		
		# Gather vertices to tuple list, and then sort them based on
		# their angle to pivot vertex. The first vertex is selected as
		# the pivot one.
		pivot = mesh.vertices[0].co * obj_m - self.pos
		vrts = []
		for vert in mesh.vertices:
			pos_abs = vert.co * obj_m
			v = pos_abs - self.pos
			angle = mathutils.AngleBetweenVecs(v, pivot)
			# Check which side of pivot this vector is
			cv = mathutils.CrossVecs(pivot, v)
			if mathutils.DotVecs(cv, self.normal) < 0:
				angle = -angle
			vrt = pos_abs, angle
			vrts.append(vrt)
		vrts.sort(key = lambda vrt: vrt[1])

		self.vrts = []
		for vrt, angle in vrts:
			self.vrts.append(vrt - self.pos)

		# Go vertices through and ensure that all of them with the next
		# and previous form triangles that has normals to same
		# direction.
		for vrts_id in range(0, len(self.vrts)):
			v = self.vrts[vrts_id]
			v_next = self.vrts[(vrts_id + 1) % len(self.vrts)]
			v_prev = self.vrts[(vrts_id - 1) % len(self.vrts)]
			normal = mathutils.CrossVecs(v_next - v, v_prev - v)
			if mathutils.DotVecs(normal, self.normal) < 0:
				raise Exception('Portal \"' + obj_name + '\" is not concave!')

	def serialize(self):
		result = bytes()

		# Rooms
		result += hpp_misc.stringToBytes(self.frontroom_name)
		result += hpp_misc.stringToBytes(self.backroom_name)

		# Position and normal
		result += hpp_misc.vectorToBytes(self.pos)
		result += hpp_misc.vectorToBytes(self.normal)

		# Vertices
		result += hpp_misc.uInt32ToBytes(len(self.vrts))
		for vrt in self.vrts:
			result += hpp_misc.vectorToBytes(vrt)

		return result




