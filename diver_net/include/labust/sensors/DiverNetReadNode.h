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
#ifndef DIVERNETREADNODE_H_
#define DIVERNETREADNODE_H_
#include <ros/ros.h>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <std_msgs/Bool.h>

#include <Eigen/Dense>

#include <string>

namespace labust
{
	namespace sensors
	{
		/**
		 * The class implements the Diver net driver.
		 * \todo Implement additional messages
		 * \todo Split acquisition and processing into two nodes.
		 * \todo Add a Diver Net imu class that will contain name, id, calibration and init options.
		 */
		class DiverNetReadNode
		{
			enum {headerSize=4, dataPerNode = 18, crc = 2, pressureData=4, temperatureData=4, hrPulseData=1};

		public:
			/**
			 * Main constructor
			 */
			DiverNetReadNode();
			/**
			 * Generic destructor.
			 */
			~DiverNetReadNode();
			/**
			 * Initialize and setup controller.
			 */
			void onInit();

		private:
			/**
			 * Handle the header detection.
			 */
			void onHeader(const boost::system::error_code& e, std::size_t size);
			/**
			 * Handle the incoming data stream.
			 */
			void onData(const boost::system::error_code& e, std::size_t size);

			/**
			 * Configure the diver net device
			 */
			void configureNet();

			/**
			 * The serial port setup helper method.
			 */
			bool setup_port();
			/**
			 * The start receive helper function.
			 */
			void start_receive();
			/**
			 * The start receive helper function.
			 */
			bool test_sync();

			/**
			 * Handle net init.
			 */
			void onNetInit(const std_msgs::Bool::ConstPtr& init);

			/**
			 * The ROS publisher.
			 */
			ros::Publisher rawData, temperature_pub, pressure_pub, hr_pulse_pub;
			/**
			 * Subscriber
			 */
			ros::Subscriber netInit;
			/**
			 * Hardware i/o service.
			 */
			boost::asio::io_service io;
			/**
			 * The serial input port.
			 */
			boost::asio::serial_port port;
			/**
			 * The main operation thread.
			 */
			boost::thread runner;
			/**
			 * The input buffer.
			 */
			boost::asio::streambuf buffer;
			/**
			 * The header ring buffer.
			 */
			std::vector<uint8_t> ringBuffer;
			/**
			 * The raw data
			 */
			std::vector<uint8_t> rawBuffer;
			/**
			 * The number of nodes.
			 */
			int nodeCount;
			/**
			 * Data mutex.
			 */
			boost::mutex dataMux;
		};
	}
}

/* DIVERNETREADNODE_H_ */
#endif
