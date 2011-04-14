#ifndef HPP_CORES_H
#define HPP_CORES_H

#include <fstream>
#include <stdint.h>


namespace Hpp
{

// Returns the amount of cores in the machine.
inline size_t getNumberOfCores(void)
{
	std::ifstream cpuinfo("/proc/cpuinfo");
	if (cpuinfo.is_open()) {
		size_t cores = 0;
		std::string line;
		while (!cpuinfo.eof()) {
			std::getline(cpuinfo, line);
			if (line.substr(0, 9) == "processor") {
				cores ++;
			}
		}
		cpuinfo.close();
		if (cores == 0) {
			cores = 1;
		}
		return cores;
	}
	return 1;
}

}

#endif
