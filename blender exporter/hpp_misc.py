# coding=UTF-8
import mathutils
import struct
import bpy

# Returns selected objects from scene
def getObjects(only_selected = False):
	result = []
	for obj in bpy.data.objects:
		if not only_selected or obj.select:
			result.append(obj)
	return result

# Functor (class that can be used as function) for comparing distances of
# vectors to some specific vector. It is also possible to give index/tuple
# index. If negative is given, then items are considered as Vectors.
class DistComparer:
	def __init__(self, v, index = -1):
		self.v = v
		self.index = index
	def __call__(self, v1, v2):
		if self.index < 0:
			v1_diff = (self.v - v1).length
			v2_diff = (self.v - v2).length
		else:
			v1_diff = (self.v - v1[self.index]).length
			v2_diff = (self.v - v2[self.index]).length
		if v1_diff < v2_diff: return -1
		if v1_diff > v2_diff: return 1
		return 0

# Returns Vector(len=3) that is perpendicular to given one
def perpVec(v):
	if abs(v.x) < abs(v.y):
		return mathutils.Vector([0, v.z, -v.y])
	else:
		return mathutils.Vector([-v.z, 0, v.x])

# Checks if two line segments at the same plane cross each others. If line
# segments are not at the same plane, then result is not reliable.
def linesegmentsIntersectAtPlane(ls0_begin, ls0_end, ls1_begin, ls1_end):
	# If linesegment #2 is totally another side of linesegment #1, then
	# they can not intersect.
	ls0 = ls0_end - ls0_begin
	v0 = ls0.cross(ls1_end - ls0_begin)
	v1 = ls0.cross(ls1_begin - ls0_begin)
	v0_v1_len = (v0 + v1).length
	if v0.length < v0_v1_len and v1.length < v0_v1_len:
		return False
	# Now check same things from linesegment #1
	ls1 = ls1_end - ls1_begin
	v0 = ls1.cross(ls0_end - ls1_begin)
	v1 = ls1.cross(ls0_begin - ls1_begin)
	v0_v1_len = (v0 + v1).length
	if v0.length < v0_v1_len and v1.length < v0_v1_len:
		return False
	return True

# Takes two axis, that define plane in three dimensional space and then convert
# any point at that plane in 3D-space to 2D space.
def transformPointToTrianglespace(pos, x_axis, y_axis):
	# Calculate helper vector that is in 90 degree against y_axis.
	helper = (x_axis.cross(y_axis)).cross(y_axis)
	result = mathutils.Vector([0, 0])

	dp_xh = x_axis.dot(helper)
	assert dp_xh != 0.0, 'transformPointToTrianglespace(): Division by zero!'
	result.x = pos.dot(helper) / dp_xh

	y_axis_abs = mathutils.Vector([abs(y_axis.x), abs(y_axis.y), abs(y_axis.z)])
	if y_axis_abs.x > y_axis_abs.y and y_axis_abs.x > y_axis_abs.z:
		assert y_axis.x != 0.0, 'transformPointToTrianglespace(): Division by zero!'
		result.y = (pos.x + -result.x * x_axis.x) / y_axis.x
	elif y_axis_abs.y > y_axis_abs.z:
		assert y_axis.y != 0.0, 'transformPointToTrianglespace(): Division by zero!'
		result.y = (pos.y + -result.x * x_axis.y) / y_axis.y
	else:
		assert y_axis.z != 0.0, 'transformPointToTrianglespace(): Division by zero!'
		result.y = (pos.z + -result.x * x_axis.z) / y_axis.z

	return result

# Returns distance to plane. If point is at the back side of plane, then
# distance is negative. Note, that distance is measured in length of
# plane_normal, so if you want it to be measured in basic units, then
# normalize plane_normal!
def distanceToPlane(point, plane_pos, plane_normal):
	assert plane_normal.length != 0.0, 'distanceToPlane(): Plane normal must not have zero length!'
	dp_nn = plane_normal.dot(plane_normal)
	assert dp_nn != 0.0, 'distanceToPlane(): Division by zero!'
	return (plane_normal.dot(point) - plane_normal.dot(plane_pos)) / dp_nn

def segmentPlaneIntersection(seg_begin, seg_end, plane_pos, plane_nrm):
	assert seg_begin != seg_end, 'Segment cannot be zero sized!'
	assert plane_nrm.length > 0.0, 'Plane normal cannot have zero length!'
	assert plane_nrm.dot(seg_end - seg_begin) != 0.0, 'Segment has same angle as plane!'
	assert plane_nrm.dot(seg_begin - plane_pos) <= 0.0 or plane_nrm.dot(seg_end - plane_pos) <= 0.0, 'Both segment ends are at the front side of plane!'
	assert plane_nrm.dot(seg_begin - plane_pos) >= 0.0 or plane_nrm.dot(seg_end - plane_pos) >= 0.0, 'Both segment ends are at the back side of plane!'
	m = (plane_pos.dot(plane_nrm) - seg_begin.dot(plane_nrm)) / plane_nrm.dot(seg_end - seg_begin)
	assert m > -0.001 and m < 1.001, 'Segment must intersect with plane!'
	return seg_begin + (seg_end - seg_begin) * m

# Returns true if hit occures and stores hit information to last two
# parameters. Return value also contains hit point and hit normal
def rayHitsTriangle(ray_begin, ray_dir, v0, v1, v2, error_tolerance = 0.0):
	assert ray_dir.length > 0.0, 'Ray dir cannot be zero!'

	tedge0 = v1 - v0
	tedge1 = v2 - v0

	hit_normal = tedge0.cross(tedge1)

	# Calculate the position at where linesegment hits the plane of
	# triangle.
	dp_e_n = ray_dir.dot(hit_normal)
	if dp_e_n == 0:
		return False, mathutils.Vector([0, 0, 0]), mathutils.Vector([0, 0, 0])
	m = (v0.dot(hit_normal) - ray_begin.dot(hit_normal)) / dp_e_n

	# If collision point is behind ray, then collision is not possible.
	if m < 0.0:
		return False, mathutils.Vector([0, 0, 0]), mathutils.Vector([0, 0, 0])

	hit_point = ray_begin + ray_dir * m

	# Find out the position in the coordinates of the plane of triangle.
	pos_at_plane = transformPointToTrianglespace(hit_point - v0, tedge0, tedge1)

	if pos_at_plane.x < -error_tolerance or pos_at_plane.y < -error_tolerance or pos_at_plane.x + pos_at_plane.y > 1 + error_tolerance:
		return False, mathutils.Vector([0, 0, 0]), mathutils.Vector([0, 0, 0])

	return True, hit_point, hit_normal

def trianglesHit(t0_v0, t0_v1, t0_v2, t1_v0, t1_v1, t1_v2, t0_bsphere_pos = None, t0_bsphere_r = None, t1_bsphere_pos = None, t1_bsphere_r = None):
	# First do bounding sphere test
	if t0_bsphere_r != None and t1_bsphere_r != None:
		if (t0_bsphere_pos - t1_bsphere_pos).length > t0_bsphere_r + t1_bsphere_r:
			return False
	t1_edge0 = t1_v1 - t1_v0
	t1_edge2 = t1_v0 - t1_v2
	if t0_bsphere_r != None:
		t1_bsphere_pos2 = t1_v0
		t1_edge0_len = t1_edge0.length
		t1_edge2_len = t1_edge2.length
		if t1_edge0_len > t1_edge2_len:
			t1_bsphere_r2 = t1_edge0_len
		else:
			t1_bsphere_r2 = t1_edge2_len
		if (t0_bsphere_pos - t1_bsphere_pos2).length > t0_bsphere_r + t1_bsphere_r2:
			return False
	t0_edge0 = t0_v1 - t0_v0
	t0_edge2 = t0_v0 - t0_v2
	if t1_bsphere_r >= 0.0:
		t0_bsphere_pos2 = t0_v0
		t0_edge0_len = t0_edge0.length
		t0_edge2_len = t0_edge2.length
		if t0_edge0_len > t0_edge2_len:
			t0_bsphere_r2 = t0_edge0_len
		else:
			t0_bsphere_r2 = t0_edge2_len
		if (t0_bsphere_pos2 - t1_bsphere_pos).length > t0_bsphere_r2 + t1_bsphere_r:
			return False
	if t0_bsphere_r < 0.0 and t1_bsphere_r < 0.0:
		t0_bsphere_pos2 = t0_v0
		t0_edge0_len = t0_edge0.length
		t0_edge2_len = t0_edge2.length
		if t0_edge0_len > t0_edge2_len:
			t0_bsphere_r2 = t0_edge0_len
		else:
			t0_bsphere_r2 = t0_edge2_len
		t1_bsphere_pos2 = t1_v0
		t1_edge0_len = t1_edge0.length
		t1_edge2_len = t1_edge2.length
		if t1_edge0_len > t1_edge2_len:
			t1_bsphere_r2 = t1_edge0_len
		else:
			t1_bsphere_r2 = t1_edge2_len
		if (t0_bsphere_pos2 - t1_bsphere_pos2).length > t0_bsphere_r2 + t1_bsphere_r2:
			return False

	# Calculate normals of both triangles
	t0_nrm = t0_edge0.cross(-t0_edge2)
	t1_nrm = t1_edge0.cross(-t1_edge2)

	# Check on which side vertices of triangles are of each others
	t0_v0_front = t1_nrm.dot(t0_v0 - t1_v0) > 0
	t0_v1_front = t1_nrm.dot(t0_v1 - t1_v0) > 0
	t0_v2_front = t1_nrm.dot(t0_v2 - t1_v0) > 0
	t1_v0_front = t0_nrm.dot(t1_v0 - t0_v0) > 0
	t1_v1_front = t0_nrm.dot(t1_v1 - t0_v0) > 0
	t1_v2_front = t0_nrm.dot(t1_v2 - t0_v0) > 0

	# Check cases when another triangle is totally at the other side of
	# another triangle.
	if (t0_v0_front == t0_v1_front and t0_v0_front == t0_v2_front) or (t1_v0_front == t1_v1_front and t1_v0_front == t1_v2_front):
		return False

	# Now we have a situation where both triangles have one of their
	# vertices at the different side of other the plane of other triangle
	# than rest of its vertices. We parse this situation to variables that
	# allows the handling in one place without many if else blocks.
	if t0_v0_front:
		if t0_v1_front:
			assert not t0_v2_front, 'Fail!'
			t0_loner = t0_v2
			t0_f0 = t0_v0
			t0_f1 = t0_v1
		else:
			if t0_v2_front:
				t0_loner = t0_v1
				t0_f0 = t0_v0
				t0_f1 = t0_v2
			else:
				t0_loner = t0_v0
				t0_f0 = t0_v1
				t0_f1 = t0_v2
	else:
		if t0_v1_front:
			if t0_v2_front:
				t0_loner = t0_v0
				t0_f0 = t0_v1
				t0_f1 = t0_v2
			else:
				t0_loner = t0_v1
				t0_f0 = t0_v0
				t0_f1 = t0_v2
		else:
			assert t0_v2_front, 'Fail!'
			t0_loner = t0_v2
			t0_f0 = t0_v0
			t0_f1 = t0_v1
	if t1_v0_front:
		if t1_v1_front:
			assert not t1_v2_front, 'Fail!'
			t1_loner = t1_v2
			t1_f0 = t1_v0
			t1_f1 = t1_v1
		else:
			if t1_v2_front:
				t1_loner = t1_v1
				t1_f0 = t1_v0
				t1_f1 = t1_v2
			else:
				t1_loner = t1_v0
				t1_f0 = t1_v1
				t1_f1 = t1_v2
	else:
		if t1_v1_front:
			if t1_v2_front:
				t1_loner = t1_v0
				t1_f0 = t1_v1
				t1_f1 = t1_v2
			else:
				t1_loner = t1_v1
				t1_f0 = t1_v0
				t1_f1 = t1_v2
		else:
			assert t1_v2_front, 'Fail!'
			t1_loner = t1_v2
			t1_f0 = t1_v0
			t1_f1 = t1_v1

	# Find out positions where edges of triangles
	# cut the plane of other triangle.
	try:
		t0_cutpos0 = segmentPlaneIntersection(t0_loner, t0_f0, t1_v0, t1_nrm)
		t0_cutpos1 = segmentPlaneIntersection(t0_loner, t0_f1, t1_v0, t1_nrm)
		t1_cutpos0 = segmentPlaneIntersection(t1_loner, t1_f0, t0_v0, t0_nrm)
		t1_cutpos1 = segmentPlaneIntersection(t1_loner, t1_f1, t0_v0, t0_nrm)
	except AssertionError:
		# TODO: Is this correct return value in this situation?
		return False

	# Check which cutpositions has longest distance to each otherse.
	# 0 = t0_cutpos0 and t0_cutpos1
	# 1 = t1_cutpos0 and t1_cutpos1
	# 2 = t0_cutpos0 and t1_cutpos1
	# 3 = t0_cutpos1 and t1_cutpos0
	# 4 = t0_cutpos0 and t1_cutpos0
	# 5 = t0_cutpos1 and t1_cutpos1
	maxdst = (t0_cutpos0 - t0_cutpos1).length
	maxdst_id = 0
	dst10_11 = (t1_cutpos0 - t1_cutpos1).length
	dst00_11 = (t0_cutpos0 - t1_cutpos1).length
	dst01_10 = (t0_cutpos1 - t1_cutpos0).length
	dst00_10 = (t0_cutpos0 - t1_cutpos0).length
	dst01_11 = (t0_cutpos1 - t1_cutpos1).length
	if dst10_11 > maxdst:
		maxdst = dst10_11
		maxdst_id = 1
	if dst00_11 > maxdst:
		maxdst = dst00_11
		maxdst_id = 2
	if dst01_10 > maxdst:
		maxdst = dst01_10
		maxdst_id = 3
	if dst00_10 > maxdst:
		maxdst = dst00_10
		maxdst_id = 4
	if dst01_11 > maxdst:
		maxdst = dst01_11
		maxdst_id = 5

	# First check situations where cutpositions of another
	# triangle are the ones that form the longest distance.
	if maxdst_id == 0 or maxdst_id == 1:
		return True
	# Rest of the situations are those where both of the furthest
	# cutpositions are from different triangles. In these situations, we
	# have to check if triangles collide or not.
	elif maxdst_id == 2:
		if (t0_cutpos0 - t0_cutpos1).length > (t0_cutpos0 - t1_cutpos0).length:
			return True
		else:
			return False
	elif maxdst_id == 3:
		if (t0_cutpos1 - t0_cutpos0).length > (t0_cutpos1 - t1_cutpos1).length:
			return True
		else:
			return False
	elif maxdst_id == 4:
		if (t0_cutpos0 - t0_cutpos1).length > (t0_cutpos0 - t1_cutpos1).length:
			return True
		else:
			return False
	else:
		assert maxdst_id == 5, 'Fail!'
		if (t0_cutpos1 - t0_cutpos0).length > (t0_cutpos1 - t1_cutpos0).length:
			return True
		else:
			return False

	# This should not be reached
	assert False, 'This should not be reached!'

def floatToBytes(f):
	return struct.pack('>f', f)

def uInt8ToBytes(i):
	result = bytes()
	result += bytes([i & 0xFF])
	return result

def uInt32ToBytes(i):
	result = bytes()
	result += bytes([(i >> 0) & 0xFF])
	result += bytes([(i >> 8) & 0xFF])
	result += bytes([(i >> 16) & 0xFF])
	result += bytes([(i >> 24) & 0xFF])
	return result

def stringToBytes(s):
	result = uInt32ToBytes(len(s))
	for c in s:
		result += bytes([ord(c)])
	return result

def vectorToBytes(v):
	result = bytes()
	result += floatToBytes(v.x)
	result += floatToBytes(v.y)
	if (len(v) >= 3):
		result += floatToBytes(v.z)
	if (len(v) == 4):
		result += floatToBytes(v.w)
	return result

def matrixToBytes(m):
	result = bytes()
	m_len = len(m)
	for row in range(0, m_len):
		for col in range(0, m_len):
			result += floatToBytes(m[row][col])
	return result

def readFloat(ifile):
	buf = ifile.read(4)
	return struct.unpack('>f', buf)[0]

def readUInt8(ifile):
	return ord(ifile.read(1)[0])

def readUInt32(ifile):
	buf = ifile.read(4)
	result = 0
	result += ord(buf[0]) << 0
	result += ord(buf[1]) << 8
	result += ord(buf[2]) << 16
	result += ord(buf[3]) << 24
	return result

def readVector(ifile, components):
	x = readFloat(ifile)
	y = readFloat(ifile)
	if components == 2:
		return mathutils.Vector([x, y])
	elif components == 3:
		z = readFloat(ifile)
		return mathutils.Vector([x, y, z])
	elif components == 4:
		z = readFloat(ifile)
		w = readFloat(ifile)
		return mathutils.Vector([x, y, z, w])
	else:
		assert False, 'Components must be 2, 3 or 4!'

def readMatrix(ifile, size):
	assert False, 'Not implemented yet!'

