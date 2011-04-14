#ifndef HPP_DATAMANAGERBASE_H
#define HPP_DATAMANAGERBASE_H

#include <string>
#include <map>

namespace Hpp
{

template< class Item >
class DatamanagerBase
{

public:

	inline DatamanagerBase(void) { }
	inline ~DatamanagerBase(void);

	inline void addItem(std::string const& name, Item* item);
	inline void deleteItem(std::string const& name);
	inline void deleteAllItems(void);

	inline bool itemExists(std::string const& name) const { return items.find(name) != items.end(); }
	inline Item* getItem(std::string const& name);

private:

	typedef std::map< std::string, Item* > Items;

	Items items;

};

template< class Item >
inline DatamanagerBase< Item >::~DatamanagerBase(void)
{
	deleteAllItems();
}

template< class Item >
inline void DatamanagerBase< Item >::addItem(std::string const& name, Item* item)
{
	typename Items::iterator items_find = items.find(name);
	if (items_find != items.end()) {
		delete items_find->second;
	}
	items[name] = item;
}

template< class Item >
inline void DatamanagerBase< Item >::deleteItem(std::string const& name)
{
	typename Items::iterator items_find = items.find(name);
	if (items_find != items.end()) {
		delete items_find->second;
	}
	items.erase(name);
}

template< class Item >
inline void DatamanagerBase< Item >::deleteAllItems(void)
{
	for (typename Items::iterator items_it = items.begin();
	     items_it != items.end();
	     items_it ++) {
	     	delete items_it->second;
	}
	items.clear();
}

template< class Item >
inline Item* DatamanagerBase< Item >::getItem(std::string const& name)
{
	typename Items::iterator items_find = items.find(name);
	if (items_find == items.end()) {
		throw Exception("\"" + name + "\" does not exist!");
	}
	return items[name];
}

}

#endif
