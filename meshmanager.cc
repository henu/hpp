#include "meshmanager.h"

#include "oldmesh.h"
#include "datamanagerbase.h"

#include "exception.h"

namespace Hpp
{

// Real container of Meshes
DatamanagerBase< OldMesh > meshes;

void Meshmanager::addMesh(std::string const& name, OldMesh* mesh)
{
	meshes.addItem(name, mesh);
}

void Meshmanager::addMeshFromRawmesh(Rawmesh const& rawmesh, std::string const& name, Rawmesh::Halvingstyle hstyle)
{
	std::string realname;
	if (!name.empty()) {
		realname = name;
	} else {
		realname = rawmesh.name;
	}
	OldMesh* new_mesh = new OldMesh(rawmesh, hstyle);
	meshes.addItem(realname, new_mesh);
}

void Meshmanager::deleteMesh(std::string const& name)
{
	meshes.deleteItem(name);
}

void Meshmanager::clear(void)
{
	meshes.deleteAllItems();
}

bool Meshmanager::meshExists(std::string const& name)
{
	return meshes.itemExists(name);
}

OldMesh* Meshmanager::getMesh(std::string const& name)
{
	return meshes.getItem(name);
}

Meshmanager::Meshmanager(void)
{
}

Meshmanager::~Meshmanager(void)
{
	meshes.deleteAllItems();
}

}
