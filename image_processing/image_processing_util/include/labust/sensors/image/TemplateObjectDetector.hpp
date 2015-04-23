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
#ifndef TEMPLATEOBJECTDETECTOR_HPP_
#define TEMPLATEOBJECTDETECTOR_HPP_
#include <labust/sensors/image/ObjectDetector.hpp>
#include <opencv2/opencv.hpp>

namespace labust {
  namespace sensors {
    namespace image {

      class TemplateObjectDetector: public ObjectDetector {

      public:
        TemplateObjectDetector();
        TemplateObjectDetector(const char *filename);
        TemplateObjectDetector(cv::Mat &img_template);
        ~TemplateObjectDetector();
        void addImageTemplate(const char *filename);
        void addImageTemplate(cv::Mat &img_template);
        virtual void detect(cv::Mat image, cv::Point2f &center, double &area);
        virtual void setEnableVideoDisplay(bool enable_video_display);

      private:
        virtual void createOpenCvWindow();
        cv::vector<cv::Mat> templates_;
      };

    }
  }
}

/* TEMPLATEOBJECTDETECTOR_HPP_ */
#endif
