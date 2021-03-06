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
#ifndef SEATRAC_DATATYPES_H
#define SEATRAC_DATATYPES_H
#include <labust/seatrac/detail/typedefs.h>
#include <boost/static_assert.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <ros/ros.h>

namespace labust
{
	namespace seatrac
	{
		#include <labust/seatrac/detail/datatype_defs.h>

	    struct DelaySpecification
	    {
	        DelaySpecification():
	            per_byte(0.08),
	            ping_duration(0.5),
	            ping_reply_duration(0.5),
	            usbl_processing_duration(0.5){};

	        void configure(ros::NodeHandle& ph)
	        {
	            ph.param("byte_delay", per_byte, per_byte);
	            ph.param("ping_duration", ping_duration, ping_duration);
	            ph.param("ping_reply_duration", ping_reply_duration, ping_reply_duration);
	            ph.param("usbl_processing_duration", usbl_processing_duration, usbl_processing_duration);
	        }

            void configureSim(ros::NodeHandle& ph)
            {
                ph.param("sim_byte_delay", per_byte, per_byte);
                ph.param("sim_ping_duration", ping_duration, ping_duration);
                ph.param("sim_ping_reply_duration", ping_reply_duration, ping_reply_duration);
                ph.param("sim_usbl_processing_duration", usbl_processing_duration, usbl_processing_duration);
            }

	        double per_byte;
	        double ping_duration;
	        double ping_reply_duration;
	        double usbl_processing_duration;
	    };
	}
}

/* SEATRAC_DATATYPES_H */
#endif
