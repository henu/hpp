#ifndef HPP_VERTEXGROUPINFLUENCES_H
#define HPP_VERTEXGROUPINFLUENCES_H

#include "real.h"
#include "renderindex.h"
#include "rendbuf.h"

#include <vector>
#include <utility>
#include <map>
#include <stdint.h>

namespace Hpp
{

/*
typedef std::vector< std::pair< uint16_t, Real > > VGInfls;
*/
typedef std::map< uint32_t, float > VGInfls;

inline VGInfls calculateFixedVertexgroupinfluence(VGInfls const& raw);

inline bool vgiNearEnough(Rendbuf< RIdx > const& vg_ids, Rendbuf< Real > const& vg_weights,
                          size_t offset4, VGInfls const& vgi, Hpp::Real max_diff_to_2);

inline VGInfls calculateFixedVertexgroupinfluence(VGInfls const& raw)
{
	if (raw.size() <= 4) {
		return raw;
	}
	std::multimap< float, uint32_t > sort_by_infl;
	for (VGInfls::const_iterator raw_it = raw.begin();
	     raw_it != raw.end();
	     ++ raw_it) {
	     	sort_by_infl.insert(std::pair< float, uint32_t >(raw_it->second, raw_it->first));
	}
	size_t skip = raw.size() - 4;
	Hpp::Real skipped_totalweight = 0;
	Hpp::Real nonskipped_totalweight = 0;
	VGInfls result;
	for (std::multimap< float, uint32_t >::const_iterator sort_by_infl_it = sort_by_infl.begin();
	     sort_by_infl_it != sort_by_infl.end();
	     ++ sort_by_infl_it) {
	     	Hpp::Real weight = sort_by_infl_it->first;
	     	if (skip > 0) {
	     		skipped_totalweight += weight;
	     		skip --;
	     	} else {
	     		nonskipped_totalweight += weight;
	     		HppAssert(result.find(sort_by_infl_it->second) == result.end(), "Already added!");
	     		result[sort_by_infl_it->second] = weight;
	     	}
	}
	// Add removed weight
	for (VGInfls::iterator result_it = result.begin();
	     result_it != result.end();
	     ++ result_it) {
	     	Hpp::Real weight = result_it->second;
	     	HppAssert(nonskipped_totalweight != 0, "Division by zero!");
	     	result_it->second += (weight / nonskipped_totalweight) * skipped_totalweight;
	}
	return result;
}

inline bool vgiNearEnough(Rendbuf< RIdx > const& vg_ids, Rendbuf< Real > const& vg_weights,
                          size_t offset4, VGInfls const& vgi, Hpp::Real max_diff_to_2)
{
	uint8_t skipped = 0;
	Hpp::Real diff = 0;
	for (uint8_t id = 0; id < 4; id ++) {
		Real weight = vg_weights[offset4 + id];
		if (weight <= 0) {
			skipped ++;
		} else {
			RIdx vg_id = vg_ids[offset4 + id];
			VGInfls::const_iterator vgi_find = vgi.find(vg_id);
			if (vgi_find == vgi.end()) {
				return false;
			}
			Real this_diff = vgi_find->second - weight;
			diff += this_diff * this_diff;
		}
	}
	if (4 - skipped != static_cast< int8_t >(vgi.size())) {
		return false;
	}
	return diff < max_diff_to_2;
}

}

#endif
