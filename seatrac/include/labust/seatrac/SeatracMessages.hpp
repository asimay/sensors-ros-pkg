/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2010, LABUST, UNIZG-FER
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the LABUST nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/
#ifndef SEATRACMESSAGES_HPP_
#define SEATRACMESSAGES_HPP_
#include <labust/preprocessor/mem_serialized_struct.hpp>
#include <boost/serialization/level.hpp>
#include <labust/seatrac/detail/vector_override.hpp>
#include <boost/static_assert.hpp>
#include <cstdint>

namespace labust
{
	namespace seatrac
	{

		struct StatusBits
		{
			bool ENVIRONMENT :1;
			bool ATTITUDE :1;
			bool MAG_CAL :1;
			bool ACC_CAL :1;
			bool AHRS_RAW_DATA :1;
			bool AHRS_COMP_DATA :1;
			bool reserved2 :1;
			bool reserved :1;
		};
		BOOST_STATIC_ASSERT(sizeof(StatusBits) == 1 && ("STATUS_BITS_T structure is assumed as size 1 byte."));
	}
};

///Define status bits as primitve type for easier deserialization
BOOST_CLASS_IMPLEMENTATION(labust::seatrac::StatusBits, boost::serialization::primitive_type);

PP_LABUST_DEFINE_BOOST_SERIALIZED_STRUCT_CLEAN((labust)(seatrac),StatusHeader,
		(StatusBits, status_bits)
		(uint64_t, timestamp))

typedef std::vector<uint8_t> PayloadType;

PP_LABUST_DEFINE_BOOST_SERIALIZED_STRUCT_CLEAN((labust)(seatrac), DatSend,
		(uint8_t, destId)
		(uint8_t, flags)
		(PayloadType, payload))

PP_LABUST_DEFINE_BOOST_SERIALIZED_STRUCT_CLEAN((labust)(seatrac), DatReceive,
		(uint8_t, srcId)
		(PayloadType, payload))



typedef int16_t vec3si[3];
typedef int32_t vec3i[3];
PP_LABUST_DEFINE_BOOST_SERIALIZED_STRUCT_CLEAN((labust)(seatrac), XcvrFix,
		(uint64_t, timestamp)
		(uint8_t, beaconId)
		(vec3si, attitude1)
		(uint16_t, vos)
		(uint8_t, range_valid)
		(uint32_t, range_count)
		(int32_t, range_time)
		(uint32_t, range_dist)
		(uint16_t, depth_local)
		(uint8_t, depth_valid)
		(uint16_t, depth_remote)
		(uint8_t, signal_valid)
		(int16_t, signal_azimuth)
		(int16_t, signal_elevation)
		(uint8_t, position_valid)
		(vec3i, position))


/* SEATRACMESSAGES_HPP_ */
#endif