#ifndef HPP_MATERIALMANAGER_H
#define HPP_MATERIALMANAGER_H

#include "rawmaterial.h"

#include <map>
#include <string>

namespace Hpp
{

// Classes that doesn't need their headers included for now
class Material;

class Materialmanager
{

public:

	// Checks if material exists
	static bool materialExists(std::string const& name);

	// Adds/deletes materials. Note, that if material already exists while
	// adding new, the old one will be deleted.
	static void addMaterial(std::string const& name, Material* material);
	static void addMaterialFromRawmaterial(Rawmaterial const& rawmat, std::string const& name = "");
	static void deleteMaterial(std::string const& name);

	// Clears the whole manager
	static void clear(void);

	// Gets specific material
	static Material* getMaterial(std::string const& name);

private:

	// Constructor and destructor in private to prevent other instances.
	Materialmanager(void);
	~Materialmanager(void);

};

}

#endif
