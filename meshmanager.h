#ifndef HPP_MESHMANAGER_H
#define HPP_MESHMANAGER_H

#include "rawmesh.h"

#include <map>
#include <string>


namespace Hpp
{

// Classes that doesn't need their headers included for now
class OldMesh;

class Meshmanager
{

public:

	// Adds/deletes meshes. Note, that if mesh already exists while adding
	// new, the old one will be deleted.
	static void addMesh(std::string const& name, OldMesh* mesh);
	static void addMeshFromRawmesh(Rawmesh const& rawmesh, std::string const& name = "", Rawmesh::Halvingstyle hstyle = Rawmesh::SHORTER);
	static void deleteMesh(std::string const& name);

	// Clears the whole manager
	static void clear(void);

	// Checks if specific mesh exists
	static bool meshExists(std::string const& name);

	// Gets specific mesh
	static OldMesh* getMesh(std::string const& name);

private:

	// Constructor and destructor in private to prevent other instances.
	Meshmanager(void);
	~Meshmanager(void);

};

}

#endif
