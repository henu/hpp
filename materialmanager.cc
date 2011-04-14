#include "materialmanager.h"

#include "genericmaterial.h"
#include "material.h"
#include "datamanagerbase.h"

#include "exception.h"

namespace Hpp
{

// Real container of Materials
DatamanagerBase< Material > materials;

bool Materialmanager::materialExists(std::string const& name)
{
	return materials.itemExists(name);
}

void Materialmanager::addMaterial(std::string const& name, Material* material)
{
	materials.addItem(name, material);
}

void Materialmanager::addMaterialFromRawmaterial(Rawmaterial const& rawmat, std::string const& name)
{
	std::string realname;
	if (!name.empty()) {
		realname = name;
	} else {
		realname = rawmat.name;
	}
	Material* new_mat = new GenericMaterial(rawmat);
	materials.addItem(realname, new_mat);
}

void Materialmanager::deleteMaterial(std::string const& name)
{
	materials.deleteItem(name);
}

void Materialmanager::clear(void)
{
	materials.deleteAllItems();
}

Material* Materialmanager::getMaterial(std::string const& name)
{
	return materials.getItem(name);
}

Materialmanager::Materialmanager(void)
{
}

Materialmanager::~Materialmanager(void)
{
	materials.deleteAllItems();
}

}
