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
#include <labust/seatrac/usbl_controller.h>
#include <labust/seatrac/seatrac_messages.h>
#include <labust/seatrac/seatrac_definitions.h>
#include <labust/seatrac/mediator.h>
#include <pluginlib/class_list_macros.h>

#include <string>

using namespace labust::seatrac;

USBLController::USBLController():
	auto_mode(false),
	enhanced_usbl(false),
	enhanced_data(false),
	is_busy(false),
	timeout(2.0),
	nextId(0)
{
	registrations[PingError::CID].push_back(boost::bind(&USBLController::onPingErrors,this,_1));
	registrations[PingSendResp::CID].push_back(boost::bind(&USBLController::onPingErrors,this,_1));
	registrations[PingResp::CID].push_back(boost::bind(&USBLController::onPingReplies,this,_1));
	registrations[DatError::CID].push_back(boost::bind(&USBLController::onPingErrors,this,_1));
	registrations[DatSendResp::CID].push_back(boost::bind(&USBLController::onPingErrors,this,_1));
	registrations[DatReceive::CID].push_back(boost::bind(&USBLController::onPingReplies,this,_1));
}

USBLController::~USBLController()
{
	this->stop();
}

bool USBLController::configure(ros::NodeHandle& nh, ros::NodeHandle& ph)
{
	//Configure desired outputs and rate
	ph.param("auto_mode",auto_mode, auto_mode);
	ph.param("enhanced_usbl", enhanced_usbl, enhanced_usbl);
	ph.param("enhanced_data", enhanced_data, enhanced_data);
	ph.param("timeout", timeout, timeout);
	ph.param("transponders", transponders, transponders);

	outSub = nh.subscribe<underwater_msgs::ModemTransmission>("outgoing",
			1, &USBLController::onOutgoing,this);
	opMode = nh.subscribe<std_msgs::Bool>("auto_mode",	1,
			&USBLController::onAutoMode,this);
	usbl_timeout = nh.advertise<std_msgs::Bool>("usbl_timeout",1);

	this->start();

	return true;
}

void USBLController::onAutoMode(const std_msgs::Bool::ConstPtr& mode)
{
	if (mode->data == auto_mode) return;
	auto_mode = mode->data;
	if (mode->data)
	{
		this->start();
	}
	else
	{
		this->stop();
	}
}

void USBLController::start()
{
	if (auto_mode)
	{
		ROS_INFO("Starting auto-mode interrogation.");
		worker = boost::thread(boost::bind(&USBLController::autorun,this));
	}
}

void USBLController::stop()
{
	ROS_INFO("Stopping auto-interrogation.");
	this->auto_mode = false;
	this->unlock();
	if (worker.joinable()) worker.join();
}

SeatracMessage::Ptr USBLController::makeDataCmd(const underwater_msgs::ModemTransmission::ConstPtr& msg,
		uint8_t msgtype)
{
	DatSendCmd::Ptr cmd(new DatSendCmd());
	cmd->dest = msg->receiver;
	cmd->msg_type = msgtype;
	cmd->data.assign(msg->payload.begin(), msg->payload.end());
	return boost::dynamic_pointer_cast<SeatracMessage>(cmd);
}

SeatracMessage::Ptr USBLController::makePingCmd(const underwater_msgs::ModemTransmission::ConstPtr& msg)
{
	PingSendCmd::Ptr cmd(new PingSendCmd());
	cmd->dest = msg->receiver;
	cmd->msg_type = (enhanced_usbl)?AMsgType::MSG_REQX : AMsgType::MSG_REQU;
	return boost::dynamic_pointer_cast<SeatracMessage>(cmd);
}

void USBLController::makeReply(const underwater_msgs::ModemTransmission::ConstPtr& msg)
{
	if (is_busy) return;
	DatQueueClearCmd::Ptr clr(new DatQueueClearCmd());
	DatQueueSetCmd::Ptr cmd(new DatQueueSetCmd());
	cmd->dest = msg->receiver;
	cmd->data.assign(msg->payload.begin(), msg->payload.end());
	this->sender(clr);
	this->sender(cmd);
}

void USBLController::onOutgoing(const underwater_msgs::ModemTransmission::ConstPtr& msg)
{
	//Sort messages by priorities ?
	SeatracMessage::Ptr message;
	bool has_data(msg->payload.size() != 0);
	//ROS_ERROR("Data size: %d", msg->payload.size());
	has_data = true;

	switch (msg->action)
	{
	case underwater_msgs::ModemTransmission::RANGING:
		//Ignore data and fall through
		has_data = false;
	case underwater_msgs::ModemTransmission::RANGING_DATA:
		//If not data do only a ping
		if (has_data)
		{
			message = makeDataCmd(msg,
					(enhanced_usbl)?AMsgType::MSG_REQX:AMsgType::MSG_REQU);
		}
		else
		{
			message = makePingCmd(msg);
		}
		break;
	case underwater_msgs::ModemTransmission::SEND_DATA:
		message = makeDataCmd(msg,
				(enhanced_data)?AMsgType::MSG_OWAYU:AMsgType::MSG_OWAY);
		break;
	case underwater_msgs::ModemTransmission::SET_REPLY:
		makeReply(msg);
		break;
	default:
		break;
	}

	//ROS_ERROR("Setup reply.");
	boost::mutex::scoped_lock l(data_mux);
	if (message != 0)
	{
		ROS_INFO("Sending message (%d->%d).", msg->sender, msg->receiver);
		//Add to queue
		outgoing.push(message);
		//Clean queue
		while (outgoing.size() > 1) outgoing.pop();
		//Only master can send data
		if (!auto_mode && !is_busy)
		{
			//Spin-off a worker thread to send one packet
			//The thread is used to avoid blocking the callback queue
			is_busy = true;
			l.unlock();
			//ROS_ERROR("Start thread.");
			worker = boost::thread(boost::bind(&USBLController::sendPkg,this));
		}
		else
		{
			ROS_WARN("Failed sending message (%d->%d). The device is busy.", msg->sender, msg->receiver);
		}
	}
}

void USBLController::sendPkg()
{
	//ROS_ERROR("Enter thread.");
	SeatracMessage::Ptr message;
	boost::mutex::scoped_lock l(data_mux);
	//Debug turn-around time measurement
	static ros::Time ltime;
	//ROS_INFO("Turnaround: %f",(ros::Time::now() - ltime).toSec());
	ltime = ros::Time::now();
	//ROS_ERROR("Check queue: %d", outgoing.size());
	if (!outgoing.empty())
	{
		message = outgoing.front();
		outgoing.pop();
	}
	l.unlock();

	//ROS_ERROR("Queue checked");

	//Send the message
	if (message != 0)
	{
		//ROS_ERROR("Message send.");
		if (!(is_busy = this->sender(message)))
		{
			ROS_WARN("USBLController: Message sending failed for CID=0x%d", message->getCid());
			return;
		}
		//ROS_ERROR("Message sent.");

		boost::mutex::scoped_lock ping_lock(ping_mux);
		boost::system_time const maxtime=boost::get_system_time()+boost::posix_time::seconds(timeout);
		while (is_busy)
		{
			if (!ping_condition.timed_wait(ping_lock,maxtime))
			{
				ROS_WARN("USBLController: USBL went into timeout.");
				std_msgs::Bool data;
				data.data = true;
				is_busy = false;
				usbl_timeout.publish(data);
				break;
			}
		}
	}

	//ROS_ERROR("Exit thread.");
}

void USBLController::autorun()
{
	ros::Rate delay(10);

	while(ros::ok() && auto_mode)
	{
		boost::mutex::scoped_lock l(data_mux);
		if (outgoing.empty())
		{
			//If nobody to ping idle and continue
			if (transponders.empty())
			{
				//Release lock before sleep.
				l.unlock();
				delay.sleep();
				continue;
			}
			ROS_INFO("USBLController: Sending ping to %d", transponders[nextId]);
			//If there is a firing sequence, fire next
			PingSendCmd::Ptr cmd(new PingSendCmd());
			cmd->dest = transponders[nextId];
			nextId = (++nextId)%transponders.size();
			cmd->msg_type = (enhanced_usbl)?AMsgType::MSG_REQX : AMsgType::MSG_REQU;
			outgoing.push(boost::dynamic_pointer_cast<SeatracMessage>(cmd));
		}
		//Release lock after update
		l.unlock();
		sendPkg();
	}
}

void USBLController::unlock()
{
	boost::mutex::scoped_lock lock(ping_mux);
	is_busy = false;
	ping_condition.notify_one();
}

bool USBLController::onPingErrors(const SeatracMessage::ConstPtr& msg)
{
	bool unlock(false);
	uint8_t cid = msg->getCid();

	if (cid == PingError::CID)
	{
		const PingError::ConstPtr err(
				boost::dynamic_pointer_cast<PingError const>(msg));
		ROS_WARN("USBLController: PingError: 0x%x", err->status);
		unlock = true;
	}
	else if (cid == PingSendResp::CID)
	{
		const PingSendResp::ConstPtr resp(
				boost::dynamic_pointer_cast<PingSendResp const>(msg));
		if ((unlock = (resp->status != CST::OK)))
				ROS_WARN("USBLController: PingSendResp: 0x%x", resp->status);
	}
	else if (cid == DatSendResp::CID)
	{
		const DatSendResp::ConstPtr resp(
				boost::dynamic_pointer_cast<DatSendResp const>(msg));
		if ((unlock = (resp->status != CST::OK)))
				ROS_WARN("USBLController: DatSendResp: 0x%x", resp->status);
	}
	else if (cid == DatError::CID)
	{
		const DatError::ConstPtr err(
				boost::dynamic_pointer_cast<DatError const>(msg));
		ROS_WARN("USBLController: DatError: 0x%x", err->status);
		unlock = true;
	}
	else
	{
		ROS_WARN("USis_busy = true;BLController: No handling for PingError: 0x%x",cid);
	}

	if (unlock)
	{
		ROS_DEBUG("USBLController: Unlocking wait.");
		this->unlock();
	}

	return true;
}

bool USBLController::onPingReplies(const SeatracMessage::ConstPtr& msg)
{
	bool unlock(false);
	uint8_t cid = msg->getCid();

	if (cid == PingResp::CID)
	{
		const PingResp::ConstPtr resp(boost::dynamic_pointer_cast<PingResp const>(msg));
		ROS_DEBUG("USBLController: Ping response received from %d.", resp->acofix.src);
	}
	else if (cid == DatReceive::CID)
	{
		const DatReceive::ConstPtr resp(boost::dynamic_pointer_cast<DatReceive const>(msg));
		ROS_DEBUG("USBLController: Data reply received from %d.", resp->acofix.src);
		//ROS_INFO("Data size: %d, Data byte 1: %d", resp->data.size(), resp->data[0]);
	}
	else
	{
		ROS_WARN("USBLController: No unhandled CID=%x",cid);
	}

	this->unlock();
	return true;
}

PLUGINLIB_EXPORT_CLASS(labust::seatrac::USBLController, labust::seatrac::DeviceController)
