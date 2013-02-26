#include "meshmanager.h"

#include "mesh.h"
#include "datamanagerbase.h"

#include "exception.h"

namespace Hpp
{

// Real container of Meshes
DatamanagerBase< Mesh > meshes;

void Meshmanager::addMesh(std::string const& name, Mesh* mesh)
{
	meshes.addItem(name, mesh);
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

Mesh* Meshmanager::getMesh(std::string const& name)
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
