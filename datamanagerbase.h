#ifndef HPP_DATAMANAGERBASE_H
#define HPP_DATAMANAGERBASE_H

#include "path.h"
#include "types.h"
#include "misc.h"
#include "texture.h"
#include "genericmaterial.h"
#include "meshloaderobj.h"
#include "mesh.h"

#include <string>
#include <map>
#include <vector>

namespace Hpp
{

class DatamanagerBase
{

public:

	inline DatamanagerBase(void) { }
	inline virtual ~DatamanagerBase(void);

	inline void scan(Path const& path);
	inline void clear(void);

	inline Texture* getTexture(std::string const& name);
	inline GenericMaterial* getMaterial(std::string const& name);
	inline Mesh* getMesh(std::string const& name, bool calculate_tangent_and_binormal = false);
	inline Meshloader* getMeshloader(std::string const& name);

protected:

	// Adds new file extension with optimal priority.
	inline void addInterestedFileextension(std::string const& ext, int priority = 0) { exts[toLower(ext)] = priority; }

private:

	// Type for found interested file. Note that this might
	// not be interested by user, but built in containers.
	struct FoundFile
	{
		std::string name;
		Path path;
		std::string extension;
		int priority;
		inline bool operator<(FoundFile const& ff) const { return this->priority > ff.priority; }
	};
	typedef std::vector< FoundFile > FoundFiles;

	// Type for container of file extensions that user is interested
	// about. The integer represents priority. The higher the value
	// is, the more early it is given to subclass.
	typedef std::map< std::string, int > Extensions;

	typedef std::map< std::string, Meshloader* > Meshloaders;
	struct TwoMeshes {
		Mesh* mesh_without_tangent_and_binormal;
		Mesh* mesh_with_tangent_and_binormal;
	};
	typedef std::map< std::string, TwoMeshes > Meshes;

	// Interested file extensions
	Extensions exts;

	FoundFiles ffs;

	// Built-in containers
	Texture::Map texs;
	GenericMaterial::Map mats;
	Meshloaders meshloaders;
	Meshes meshes;

	inline void scanRecursively(Path const& path, std::string const& name_prefix);

	inline void clearBuiltInData(void);

	// Gives interested file to subclass for handling.
	virtual void handleFile(std::string const& name, std::string const& extension, Path const& path) = 0;

	// Clear all items
	virtual void doClear(void) = 0;

	// Gets container of textures. This is used by built-in functions
	inline virtual Texture::Map getTextures(void) { return texs; }

	// Built-in handlers
	inline void handleBuiltInFile(std::string const& name, std::string const& extension, Path const& path);
	inline void handleTextureFile(std::string name, Path const& path);
	inline void handleMeshFile(std::string const& name, Path const& path);
	inline void handleMaterialFile(std::string const& name, Path const& path);

};

inline DatamanagerBase::~DatamanagerBase(void)
{
	clearBuiltInData();
}

inline void DatamanagerBase::scan(Path const& path)
{
	clear();

	scanRecursively(path, "");
	// Inform subclass and baseclass about found files.
	// But first sort the list based on priority.
	std::sort(ffs.begin(), ffs.end());
	for (FoundFiles::const_iterator ffs_it = ffs.begin();
	     ffs_it != ffs.end();
	     ++ ffs_it) {
		FoundFile const& ff = *ffs_it;
		// If subclass is interested about this
		if (exts.find(ff.extension) != exts.end()) {
			handleFile(ff.name, ff.extension, ff.path);
		}
		// Baseclass is interested about his
		else {
			handleBuiltInFile(ff.name, ff.extension, ff.path);
		}
	}
	// Clear found files
	ffs.clear();
}

inline void DatamanagerBase::clear(void)
{
	clearBuiltInData();
	doClear();
}

inline Texture* DatamanagerBase::getTexture(std::string const& name)
{
	Texture::Map::iterator texs_find = texs.find(name);
	if (texs_find == texs.end()) throw Hpp::Exception("Texture \"" + name + "\" not found!");
	return texs_find->second;
}

inline GenericMaterial* DatamanagerBase::getMaterial(std::string const& name)
{
	GenericMaterial::Map::iterator mats_find = mats.find(name);
	if (mats_find == mats.end()) throw Hpp::Exception("Material \"" + name + "\" not found!");
	return mats_find->second;
}

inline Mesh* DatamanagerBase::getMesh(std::string const& name, bool calculate_tangent_and_binormal)
{
	Meshes::iterator meshes_find = meshes.find(name);
	if (meshes_find != meshes.end()) {
		TwoMeshes& twomeshes = meshes_find->second;
		if (calculate_tangent_and_binormal && twomeshes.mesh_with_tangent_and_binormal) {
			return twomeshes.mesh_with_tangent_and_binormal;
		}
		if (!calculate_tangent_and_binormal && twomeshes.mesh_without_tangent_and_binormal) {
			return twomeshes.mesh_without_tangent_and_binormal;
		}
	}
	Meshloaders::iterator meshloaders_find = meshloaders.find(name);
	if (meshloaders_find == meshloaders.end()) throw Hpp::Exception("Mesh \"" + name + "\" not found!");
	Meshloader* meshloader = meshloaders_find->second;
	if (meshes_find == meshes.end()) {
		TwoMeshes new_twomeshes;
		new_twomeshes.mesh_without_tangent_and_binormal = NULL;
		new_twomeshes.mesh_with_tangent_and_binormal = NULL;
		meshes[name] = new_twomeshes;
		meshes_find = meshes.find(name);
	}
	TwoMeshes& twomeshes = meshes_find->second;
	if (calculate_tangent_and_binormal) {
		twomeshes.mesh_with_tangent_and_binormal = meshloader->createMesh(true);
		return twomeshes.mesh_with_tangent_and_binormal;
	}
	twomeshes.mesh_without_tangent_and_binormal = meshloader->createMesh(false);
	return twomeshes.mesh_without_tangent_and_binormal;
}

inline Meshloader* DatamanagerBase::getMeshloader(std::string const& name)
{
	Meshloaders::iterator meshloaders_find = meshloaders.find(name);
	if (meshloaders_find == meshloaders.end()) throw Hpp::Exception("Meshloader \"" + name + "\" not found!");
	return meshloaders_find->second;
}

inline void DatamanagerBase::scanRecursively(Path const& path, std::string const& name_prefix)
{
	Path::DirChildren children;
	path.listDir(children);
	for (Path::DirChildren::const_iterator children_it = children.begin();
	     children_it != children.end();
	     ++ children_it) {
		Path::DirChild const& child = *children_it;
		if (child.type == Path::DIRECTORY) {
			scanRecursively(path / child.name, name_prefix + child.name + "/");
		} else if (child.type != Path::SYMLINK) {
			std::string child_name_lowcase = toLower(child.name);
			// Go all interested file extensions through and check if child matches with any of them
			bool match_found = false;
			for (Extensions::const_iterator exts_it = exts.begin();
			     exts_it != exts.end();
			     ++ exts_it) {
				std::string const& ext = exts_it->first;
				if (endsTo(child_name_lowcase, "." + ext)) {
					std::string item_name = name_prefix + child.name.substr(0, child.name.size() - ext.size() - 1);
					FoundFile new_ff;
					new_ff.name = item_name;
					new_ff.extension = ext;
					new_ff.path = path / child.name;
					new_ff.priority = exts_it->second;
					ffs.push_back(new_ff);
					match_found = true;
					break;
				}
			}
			// If no match was found, then check built-in interested file extensions
			if (!match_found) {
				std::string ext = "";
				if (endsTo(child_name_lowcase, ".jpeg")) ext = "jpeg";
				else if (endsTo(child_name_lowcase, ".jpg")) ext = "jpg";
				else if (endsTo(child_name_lowcase, ".png")) ext = "png";
				else if (endsTo(child_name_lowcase, ".obj")) ext = "obj";
				else if (endsTo(child_name_lowcase, ".material")) ext = "material";

				if (ext != "") {
					std::string item_name = name_prefix + child.name.substr(0, child.name.size() - ext.size() - 1);
					FoundFile new_ff;
					new_ff.name = item_name;
					new_ff.extension = ext;
					new_ff.path = path / child.name;
					// Textures
					if (ext == "jpeg" || ext == "jpg" || ext == "png") {
						new_ff.priority = 100;
					}
					// Meshes
					else if (endsTo(child_name_lowcase, "obj")) {
						new_ff.priority = -100;
					}
					// Materials
					else if (endsTo(child_name_lowcase, "material")) {
						new_ff.priority = 0;
					}
					ffs.push_back(new_ff);
				}
			}
		}
	}
}

inline void DatamanagerBase::clearBuiltInData(void)
{
	for (Texture::Map::iterator texs_it = texs.begin();
	     texs_it != texs.end();
	     ++ texs_it) {
		Texture* tex = texs_it->second;
		delete tex;
	}
	texs.clear();
	for (GenericMaterial::Map::iterator mats_it = mats.begin();
	     mats_it != mats.end();
	     ++ mats_it) {
		GenericMaterial* mat = mats_it->second;
		delete mat;
	}
	mats.clear();
	for (Meshloaders::iterator meshloaders_it = meshloaders.begin();
	     meshloaders_it != meshloaders.end();
	     ++ meshloaders_it) {
		Meshloader* meshloader = meshloaders_it->second;
		delete meshloader;
	}
	meshloaders.clear();
	for (Meshes::iterator meshes_it = meshes.begin();
	     meshes_it != meshes.end();
	     ++ meshes_it) {
		TwoMeshes& twomeshes = meshes_it->second;
		delete twomeshes.mesh_without_tangent_and_binormal;
		delete twomeshes.mesh_with_tangent_and_binormal;
	}
	meshes.clear();
}

inline void DatamanagerBase::handleBuiltInFile(std::string const& name, std::string const& extension, Path const& path)
{
	if (extension == "jpeg" || extension == "jpg" || extension == "png") {
		handleTextureFile(name, path);
	} else if (extension == "obj") {
		handleMeshFile(name, path);
	} else if (extension == "material") {
		handleMaterialFile(name, path);
	}
}

inline void DatamanagerBase::handleTextureFile(std::string name, Path const& path)
{
	Texture::Flags tex_flags = 0;
	// Check for texture flags from file names
	while (true) {
		if (endsTo(toLower(name), "_REMOVEALPHA")) {
			name = name.substr(0, name.size() - strlen("_REMOVEALPHA"));
			tex_flags |= Texture::REMOVE_ALPHA;
			continue;
		} else if (endsTo(toLower(name), "_FORCEALPHA")) {
			name = name.substr(0, name.size() - strlen("_FORCEALPHA"));
			tex_flags |= Texture::FORCE_ALPHA;
			continue;
		} else if (endsTo(toLower(name), "_FORCEGRAYSCALE")) {
			name = name.substr(0, name.size() - strlen("_FORCEGRAYSCALE"));
			tex_flags |= Texture::FORCE_GRAYSCALE;
			continue;
		} else if (endsTo(toLower(name), "_CLAMPTOEDGEHORIZONTALLY")) {
			name = name.substr(0, name.size() - strlen("_CLAMPTOEDGEHORIZONTALLY"));
			tex_flags |= Texture::CLAMP_TO_EDGE_HORIZONTALLY;
			continue;
		} else if (endsTo(toLower(name), "_CLAMPTOEDGEVERTICALLY")) {
			name = name.substr(0, name.size() - strlen("_CLAMPTOEDGEVERTICALLY"));
			tex_flags |= Texture::CLAMP_TO_EDGE_VERTICALLY;
			continue;
		} else if (endsTo(toLower(name), "_CLAMPTOEDGE")) {
			name = name.substr(0, name.size() - strlen("_CLAMPTOEDGE"));
			tex_flags |= Texture::CLAMP_TO_EDGE;
			continue;
		}
		break;
	}
	Texture* new_tex = new Texture(path, DEFAULT, tex_flags);
	Texture::Map::iterator texs_find = texs.find(name);
	if (texs_find != texs.end()) {
		delete texs_find->second;
	}
	texs[name] = new_tex;
}

inline void DatamanagerBase::handleMeshFile(std::string const& name, Path const& path)
{
	MeshloaderObj* new_meshloader = new MeshloaderObj(path);
	Meshloaders::iterator meshloaders_find = meshloaders.find(name);
	if (meshloaders_find != meshloaders.end()) {
		delete meshloaders_find->second;
	}
	Meshes::iterator meshes_find = meshes.find(name);
	if (meshes_find != meshes.end()) {
		TwoMeshes& twomeshes = meshes_find->second;
		delete twomeshes.mesh_without_tangent_and_binormal;
		delete twomeshes.mesh_with_tangent_and_binormal;
	}
	meshloaders[name] = new_meshloader;
}

inline void DatamanagerBase::handleMaterialFile(std::string const& name, Path const& path)
{
	GenericMaterial* new_mat = new GenericMaterial(path, getTextures(), false);
	GenericMaterial::Map::iterator mats_find = mats.find(name);
	if (mats_find != mats.end()) {
		delete mats_find->second;
	}
	mats[name] = new_mat;
}

}

#endif
