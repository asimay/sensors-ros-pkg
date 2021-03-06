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
 *  Created: 23.05.2016.
 *  Author: Ivor Rendulic
 *********************************************************************/
#ifndef MAGNETOMETERCALIBRATION_HPP_
#define MAGNETOMETERCALIBRATION_HPP_

#include <Eigen/Dense>

namespace labust {
  namespace sensors {

    /**
     * Ellipsoid fit, based on Yury Petrov's Matlab script.
     * http://www.mathworks.com/matlabcentral/fileexchange/24693-ellipsoid-fit
     */
    void ellipsoidFit(const Eigen::MatrixXd& data,
        Eigen::Vector3d& center,
        Eigen::Vector3cd& evals,
        Eigen::Matrix3cd& evecs) {
      Eigen::MatrixXd D = Eigen::MatrixXd::Zero(data.rows(), 9);
      Eigen::VectorXd x = data.col(0);
      Eigen::VectorXd y = data.col(1);
      Eigen::VectorXd z = data.col(2);
      Eigen::VectorXd xx = x.cwiseProduct(x);
      Eigen::VectorXd yy = y.cwiseProduct(y);
      Eigen::VectorXd zz = z.cwiseProduct(z);

      D.col(0) = xx + yy - 2 * zz;
      D.col(1) = xx + zz - 2 * yy;
      D.col(2) = 2 * x.cwiseProduct(y);
      D.col(3) = 2 * x.cwiseProduct(z);
      D.col(4) = 2 * y.cwiseProduct(z);
      D.col(5) = 2 * x;
      D.col(6) = 2 * y;
      D.col(7) = 2 * z;
      D.col(8) = Eigen::VectorXd::Constant(x.rows(), 1.0);
      Eigen::VectorXd d2 = xx + yy + zz;
      Eigen::VectorXd u = (D.transpose() * D).colPivHouseholderQr().solve(D.transpose() * d2);
      Eigen::VectorXd v(u.rows()+1);
      v(0) = u(0) + u(1) - 1;
      v(1) = u(0) - 2 * u(1) - 1;
      v(2) = u(1) - 2 * u(0) - 1;
      v.segment(3,7) = u.segment(2,7);

      v = v.transpose();
      Eigen::MatrixXd A(4,4);
      A << v(0), v(3), v(4), v(6),
           v(3), v(1), v(5), v(7),
           v(4), v(5), v(2), v(8),
           v(6), v(7), v(8), v(9);
      center = -A.block<3,3>(0,0).colPivHouseholderQr().solve(v.segment(6,3));

      Eigen::MatrixXd T = Eigen::MatrixXd::Identity(4,4);
      T.row(3).segment(0,3) = center;

      Eigen::MatrixXd R = T * A * T.transpose();
      Eigen::EigenSolver<Eigen::MatrixXd> es;
      es.compute(R.block<3,3>(0,0) / -R(3,3));

      evals = es.eigenvalues();
      evecs = es.eigenvectors();
    }

    struct MagnetometerCalibrationData {
      Eigen::Vector3d center;
      Eigen::Vector3d radii;
      Eigen::Vector3cd evals;
      Eigen::Matrix3cd evecs;
      Eigen::Matrix3d calib_matrix;
      double scale;
      double avg_sqerror;
      double sphere_coverage;
    };

    MagnetometerCalibrationData getMagnetometerCalibrationData(const Eigen::MatrixXd& data) {
      Eigen::Vector3d center;
      Eigen::Vector3cd evals;
      Eigen::Matrix3cd evecs;
      ellipsoidFit(data, center, evals, evecs);

      Eigen::Vector3d revals = evals.real();
      Eigen::Matrix3d revecs = evecs.real();
     
      Eigen::Vector3d sqrevals;
      for (int i=0; i<sqrevals.rows(); ++i) {
        sqrevals(i) = std::sqrt(revals(i));
      }
      Eigen::Vector3d radii;
      Eigen::Vector3d sgns;
      radii = revals.cwiseAbs().cwiseInverse().cwiseSqrt();
      for (int i=0; i<radii.rows(); ++i) {
        if (revals(i) < 0) {
          radii(i) *= -1;
          sgns(i) = -1;
        } else {
          sgns(i) = 1;
        }
      }

      double scale = std::pow(radii(0) * radii(1) * radii(2), 1.0/3);

      Eigen::MatrixXd d(data.rows(), data.cols());
      for (int i=0; i<data.rows(); ++i) {
        d.row(i) = data.row(i) - center.transpose();
      }

      // Calculate total squared error chi2
      d = d * revecs;
      double chi2 = 0.0;
      for (int i=0; i<d.rows(); ++i) {
        d.row(i) = d.row(i).cwiseQuotient(radii.transpose());
        d.row(i) = d.row(i).cwiseProduct(d.row(i));
        chi2 += std::abs(1 - d.row(i).dot(sgns)); 
      }

      // Calculate coverage of the sphere
      std::set<std::pair<int,int> > sphere_coverage;
      const int theta_quant_size = 18;
      const int phi_quant_size = 9;
      for (int i=0; i<data.rows(); ++i) {
        double x = data.row(i)(0);
        double y = data.row(i)(1);
        double z = data.row(i)(2);
        double r = std::sqrt(x*x + y*y + z*z);
        double theta = std::atan2(y,x) * 180 / M_PI;
        double phi = std::acos(z/r) * 180 / M_PI;
        int theta_quant = std::round(theta * theta_quant_size / 360);
        int phi_quant = std::round(phi * phi_quant_size / 180);
        sphere_coverage.insert(std::make_pair(theta_quant, phi_quant));
      }

      MagnetometerCalibrationData mcd;
      mcd.center = center;
      mcd.radii = radii;
      mcd.evals = evals;
      mcd.evecs = evecs;
      mcd.calib_matrix = revecs * sqrevals.asDiagonal() * revecs.inverse() * scale;
      mcd.avg_sqerror = chi2 / data.rows();
      mcd.sphere_coverage = static_cast<double>(sphere_coverage.size()) / 
        (theta_quant_size * phi_quant_size);

      return mcd;
    }

    Eigen::Vector3d calibrateMagnetometer(const Eigen::Vector3d& data, 
        const MagnetometerCalibrationData& mcd) {
      Eigen::Vector3d res = data - mcd.center;
      return mcd.calib_matrix * res;
    }

  }
}

/* MAGNETOMETERCALIBRATION_HPP_ */
#endif
