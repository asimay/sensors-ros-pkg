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
 *  Author : Ivor Rendulic
 *  Created: 15.03.2015. 
 *********************************************************************/
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/image_encodings.h>
#include <underwater_msgs/SonarBathymetry.h>

#include <labust/sensors/image/SonarImageUtil.hpp>
#include <labust/sensors/image/ArisSonar.hpp>
#include <labust/sensors/image/BathymetryNode.hpp>
#include <labust/sensors/image/ImageProcessingUtil.hpp>

#include <opencv2/opencv.hpp>


using namespace labust::sensors::image;


BathymetryNode::BathymetryNode() : 
    it(nh),
    altitude_offset(0) {
  ros::NodeHandle ph("~");
  ph.getParam("altitude_offset", altitude_offset);
  this->onInit();
};

BathymetryNode::~BathymetryNode() {};

void BathymetryNode::onInit() {
  ros::Rate rate(1);
  sonar_info_sub = nh.subscribe("/soundmetrics_aris3000/sonar_info", 1, &BathymetryNode::setSonarInfo, this);
  image_sub = it.subscribe("/soundmetrics_aris3000/polar", 1, &BathymetryNode::setSonarImage, this);
  sonar_bathymetry_pub = nh.advertise<underwater_msgs::SonarBathymetry>("bathymetry", 1);
  sonar_altitude_pub = nh.advertise<std_msgs::Float32>("altitude", 1);
}

// Calculates bearings for each of the sonar beams.
// ARIS has nbeams=128, with horizontal FOV of 28 deg.
void BathymetryNode::recalculateBearings(int nbeams) {
  bearing.resize(nbeams);
  bearing[0] = -14.0+14.0/nbeams;
  double step = 28.0/nbeams;
  for (int i=1; i<nbeams; ++i) {
    bearing[i] = bearing[i-1]+step;
  }

}

// Callback for sonar info message. Stores it and, if there is already
// a matching sonar image with same timestamp, calls for processing.
void BathymetryNode::setSonarInfo(const underwater_msgs::SonarInfo::ConstPtr &msg) {
  if (msg->beams != aris.getSonarInfo().beams) {
    recalculateBearings(msg->beams);    
  }
  aris.saveSonarInfo(*msg);
  cv_bridge::CvImagePtr frame = aris.getSonarImage();
  if (frame == 0) return;
  if (frame->header.stamp == msg->header.stamp) {
    processFrame();
  }
}

// Callback for sonar image message. Stores it and, if there is already 
// a matching sonar info message with same timestamp, calls for processing.
void BathymetryNode::setSonarImage(const sensor_msgs::Image::ConstPtr &img) {
  aris.saveSonarImage(img);
  underwater_msgs::SonarInfo si = aris.getSonarInfo();
  if (img->header.stamp == si.header.stamp) {
    processFrame();
  }
}

// Main processing function.
void BathymetryNode::processFrame() {
  // Transform image to cvBridge format.
  cv_bridge::CvImagePtr cv_image_bgr = aris.getSonarImage();
  
  // Provide the altitude estimator the sonar info.
  underwater_msgs::SonarInfo si = aris.getSonarInfo();
  sonar_altitude_estimator.setSonarInfo(si);
  
  // Call process method with the sonar image in opencv format. Result are ranges for each beam.
  std::vector<double> bath = sonar_altitude_estimator.process(cv_image_bgr->image.clone());
  
  // min_alt is closest point to the bottom (minimum range accross all beams).
  double min_alt = *(std::min_element(bath.begin(), bath.end()));

  double max_alt = 0;
  underwater_msgs::SonarBathymetry::Ptr sonar_bathymetry(new underwater_msgs::SonarBathymetry);
  std_msgs::Float32::Ptr sonar_altitude(new std_msgs::Float32);
  
  // For altitude use min_alt.
  sonar_altitude->data = min_alt + altitude_offset;
  // For bathymetry use data received by sonar_altitude_estimator.
  sonar_bathymetry->range.resize(bath.size());
  sonar_bathymetry->bearing = bearing;
  for (int i=0; i<bath.size(); ++i) {
    if (bath[i] > max_alt && bath[i] < 20.0) {
      max_alt = bath[i] + altitude_offset;
    }
    sonar_bathymetry->range[i] = bath[i] + altitude_offset;
  }

  // Optional - live sonar ranging.
  if (min_alt != 0 && max_alt != 0) {
    //aris.setRangeOfInterest(min_alt-altitude_offset, max_alt-altitude_offset);
    //aris.setRangeOfInterest(0.3, max_alt-altitude_offset);
  }

  // Add timestamp and publish bathymetry and altitude.
  sonar_bathymetry->header.stamp = si.header.stamp;
  sonar_bathymetry_pub.publish(sonar_bathymetry);
  sonar_altitude_pub.publish(sonar_altitude);
}

int main(int argc, char **argv) {

  ros::init(argc, argv, "sonar_bathymetry_node"); 
  BathymetryNode node; 
  ros::spin();

  return 0;
}
