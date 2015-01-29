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
*
*  Author: Dula Nad
*  Created: 02.04.2013.
*********************************************************************/
#ifndef USBLFILTER_HPP_
#define USBLFILTER_HPP_
#include <labust/navigation/KFCore.hpp>
#include <labust/navigation/KinematicModel.hpp>

#include <auv_msgs/NavSts.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>
#include <geometry_msgs/PointStamped.h>
#include <nodelet/nodelet.h>
#include <ros/ros.h>

#include <boost/thread.hpp>

#include <string>

namespace labust
{
	namespace tritech
	{
		/**
		 * The class implements the Tritech USBL acquisition nodelet.
		 */
		class USBLFilter : public nodelet::Nodelet
		{
			typedef labust::navigation::KFCore<labust::navigation::KinematicModel> KFilter;
		public:
			/**
			 * Default constructor.
			 */
			USBLFilter();
			/**
			 * Default destructor.
			 */
			~USBLFilter();

			/**
			 * Node initialization.
			 */
			void onInit();
			/**
			 * The main run method.
			 */
			void run();

		protected:
			/**
			 * Handles arrived USBL navigation messages.
			 */
			void onUsbl(const geometry_msgs::PointStamped::ConstPtr& msg);
			/**
			 * Handles arrived USBL navigation messages.
			 */
			void onUsblNavSts(const auv_msgs::NavSts::ConstPtr& msg);
			/**
			 * Configure from ROS file.
			 */
			void configureModel(ros::NodeHandle& nh);
			/**
			 * Propagates the USBL model.
			 */
			void step(KFilter::input_type& vec);

			/**
			 * The USBL device.
			 */
			KFilter filter;
			/**
			 * The depth state.
			 */
			double depth;
			/**
			 * The data and condition mux.
			 */
			boost::mutex dataMux;
			/**
			 * The worker thread.
			 */

			boost::thread worker;

			/**
			 * The navigation publisher.
			 */
			ros::Publisher navPub;
			/**
			 * The usbl measurement subscriber.
			 */
			ros::Subscriber usblSub, usblNavStsSub;
			/**
			 * Transform buffer.
			 */
			tf2_ros::Buffer buffer;
			/**
			 * Frame transform listener.
			 */
			tf2_ros::TransformListener listener;
			/**
			 * Safety timeout.
			 */
			int timeout, iteration;
			/**
			 * Maximum diver speed.
			 */
			double maxSpeed;
			/**
			 * Pass through configuration
			 */
			bool isPassThrough;
			/**
			 * Test for received position.
			 */
			bool hasUSBL;
			///The last state.
			KFilter::vector xd;
			///The last covariance.
			KFilter::matrix Pd;
		};
	}
}

/* USBLNODELET_HPP_ */
#endif



