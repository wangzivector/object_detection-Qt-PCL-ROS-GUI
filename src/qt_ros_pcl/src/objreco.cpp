/*
 * objreco.cpp
 *
 *  Created on: May 13, 2020
 *      Author: wangzivector
 * This cppfile mass with pointcloud objection
 * recognition process
 */
#include "objreco.h"

ObjReco::ObjReco()
{
  cloud_world = PointCloud::Ptr(new PointCloud());
  cloud_pointRGBNormal = PointRGBNormalCloud::Ptr(new PointRGBNormalCloud());
  //
  // filters param
  //
  axisFilter_axis_size = 0.2f;
  gridFilter_grid_size = 0.002f;
  planeFilter_threshold_plane = 0.008;
  outlierFilter_outlier_meanK = 30;
  outlierFilter_outlier_Thresh = 0.1;
  outlierFilter_noise_filter = 1;
  backgroundFilter_resolution = 0.015;

  /// param keypoints
  downSample_sample_radius = 0.005;

  /// param segmetation
  rgbSegmentation_dist_thre = 3;
  rgbSegmentation_point_color_thre = 13;
  rgbSegmentation_region_color_thre = 30;
  rgbSegmentation_min_cluster = 350;

  /// param reconstruction
  mlsReconstruction_search_radius = 0.03;

  /// param feature
  shot352Estimation_descr_rad_352 = 0.03;
  shot1344Estimation_descr_rad_1344 = 0.03;

  /// param registration
  RANSA_max_iterations = 200;
  RANSA_number_samples = 20;
  RANSA_randomness = 10;
  RANSA_similar_thre = 0.9f;
  RANSA_max_corr_distance = 0.015;
  RANSA_min_sample_distance = 0.25;

  NDT_transepsilon = 0.001;
  NDT_stepsize = 0.1;
  NDT_resolution = 1;
  NDT_maxiteration = 20;

  SACIA_number_samples = 20;
  SACIA_randomness = 10;
  SACIA_min_sample_distance = 0.25;
  SACIA_max_correspondence_distance = 0.015;
  SACIA_max_iterations = 200;

  ICP_max_corr_distance = 0.08;
  ICP_max_iter_icp = 100000;
  ICP_transformation = 1;
  ICP_euclidean_Fitness = 0.002;
}

//===================================================
//  checkReconstruction
//  take a look whether recongstruction works well
//===================================================
bool ObjReco::checkReconstruction()
{
  if (cloud_world->size() == 0)
  {
    std::cout << "processing pointcloud doest be loaded yet.";
    return false;
  }
  return mlsReconstruction(cloud_world, cloud_pointRGBNormal, 0.1);
}

//===================================================
//  saveIni
//  save the param of obj reco tasks
//===================================================
bool ObjReco::saveIni()
{
  QSettings* Inifile = new QSettings(
      "/home/wang/catkin_qtws/src/qt_ros_pcl/resources/config.ini",
      QSettings::IniFormat);

  /// param filters
  Inifile->setValue("axisFilter_axis_size", axisFilter_axis_size);
  Inifile->setValue("gridFilter_grid_size", gridFilter_grid_size);
  Inifile->setValue("planeFilter_threshold_plane", planeFilter_threshold_plane);
  Inifile->setValue("outlierFilter_outlier_meanK", outlierFilter_outlier_meanK);
  Inifile->setValue("outlierFilter_outlier_Thresh",
                    outlierFilter_outlier_Thresh);
  Inifile->setValue("outlierFilter_noise_filter", outlierFilter_noise_filter);
  Inifile->setValue("backgroundFilter_resolution", backgroundFilter_resolution);

  /// param keypoints
  Inifile->setValue("downSample_sample_radius", downSample_sample_radius);

  /// param segmetation
  Inifile->setValue("rgbSegmentation_dist_thre", rgbSegmentation_dist_thre);
  Inifile->setValue("rgbSegmentation_point_color_thre",
                    rgbSegmentation_point_color_thre);
  Inifile->setValue("rgbSegmentation_region_color_thre",
                    rgbSegmentation_region_color_thre);
  Inifile->setValue("rgbSegmentation_min_cluster", rgbSegmentation_min_cluster);

  /// param reconstruction
  Inifile->setValue("mlsReconstruction_search_radius",
                    mlsReconstruction_search_radius);

  /// param feature
  Inifile->setValue("shot352Estimation_descr_rad_352",
                    shot352Estimation_descr_rad_352);
  Inifile->setValue("shot1344Estimation_descr_rad_1344",
                    shot1344Estimation_descr_rad_1344);

  /// param registration
  Inifile->setValue("RANSA_max_iterations", RANSA_max_iterations);
  Inifile->setValue("RANSA_number_samples", RANSA_number_samples);
  Inifile->setValue("RANSA_randomness", RANSA_randomness);
  Inifile->setValue("RANSA_similar_thre", RANSA_similar_thre);
  Inifile->setValue("RANSA_max_corr_distance", RANSA_max_corr_distance);
  Inifile->setValue("RANSA_min_sample_distance", RANSA_min_sample_distance);

  Inifile->setValue("NDT_transepsilon", NDT_transepsilon);
  Inifile->setValue("NDT_stepsize", NDT_stepsize);
  Inifile->setValue("NDT_resolution", NDT_resolution);
  Inifile->setValue("NDT_maxiteration", NDT_maxiteration);

  Inifile->setValue("SACIA_number_samples", SACIA_number_samples);
  Inifile->setValue("SACIA_randomness", SACIA_randomness);
  Inifile->setValue("SACIA_min_sample_distance", SACIA_min_sample_distance);
  Inifile->setValue("SACIA_max_correspondence_distance",
                    SACIA_max_correspondence_distance);
  Inifile->setValue("SACIA_max_iterations", SACIA_max_iterations);

  Inifile->setValue("ICP_max_corr_distance", ICP_max_corr_distance);
  Inifile->setValue("ICP_max_iter_icp", ICP_max_iter_icp);
  Inifile->setValue("ICP_transformation", ICP_transformation);
  Inifile->setValue("ICP_euclidean_Fitness", ICP_euclidean_Fitness);

  std::cout << "saved params" << std::endl;
  delete Inifile;
  return true;
}

//===================================================
//  loadIni
//  load ini when start this object.
//===================================================
bool ObjReco::loadIni(bool reset)
{
  QSettings* Inifile;
  if (!reset)
    Inifile = new QSettings(
        "/home/wang/catkin_qtws/src/qt_ros_pcl/resources/config.ini",
        QSettings::IniFormat);
  else
    Inifile = new QSettings(
        "/home/wang/catkin_qtws/src/qt_ros_pcl/resources/config_base.ini",
        QSettings::IniFormat);
  //  int intLock = Inifile->value("lockTime").toInt();
  /// param filters
  axisFilter_axis_size = Inifile->value("axisFilter_axis_size").toFloat();
  gridFilter_grid_size = Inifile->value("gridFilter_grid_size").toFloat();
  planeFilter_threshold_plane =
      Inifile->value("planeFilter_threshold_plane").toDouble();
  outlierFilter_outlier_meanK =
      Inifile->value("outlierFilter_outlier_meanK").toInt();
  outlierFilter_outlier_Thresh =
      Inifile->value("outlierFilter_outlier_Thresh").toDouble();
  outlierFilter_noise_filter =
      Inifile->value("outlierFilter_noise_filter").toInt();
  backgroundFilter_resolution =
      Inifile->value("backgroundFilter_resolution").toDouble();

  /// param keypoints
  downSample_sample_radius =
      Inifile->value("downSample_sample_radius").toDouble();

  /// param segmetation
  rgbSegmentation_dist_thre =
      Inifile->value("rgbSegmentation_dist_thre").toFloat();
  rgbSegmentation_point_color_thre =
      Inifile->value("rgbSegmentation_point_color_thre").toFloat();
  rgbSegmentation_region_color_thre =
      Inifile->value("rgbSegmentation_region_color_thre").toFloat();
  rgbSegmentation_min_cluster =
      Inifile->value("rgbSegmentation_min_cluster").toInt();

  /// param reconstruction
  mlsReconstruction_search_radius =
      Inifile->value("mlsReconstruction_search_radius").toDouble();

  /// param feature
  shot352Estimation_descr_rad_352 =
      Inifile->value("shot352Estimation_descr_rad_352").toDouble();
  shot1344Estimation_descr_rad_1344 =
      Inifile->value("shot1344Estimation_descr_rad_1344").toDouble();

  /// param registration
  RANSA_max_iterations = Inifile->value("RANSA_max_iterations").toInt();
  RANSA_number_samples = Inifile->value("RANSA_number_samples").toInt();
  RANSA_randomness = Inifile->value("RANSA_randomness").toInt();
  RANSA_similar_thre = Inifile->value("RANSA_similar_thre").toFloat();
  RANSA_max_corr_distance =
      Inifile->value("RANSA_max_corr_distance").toDouble();
  RANSA_min_sample_distance =
      Inifile->value("RANSA_min_sample_distance").toFloat();

  NDT_transepsilon = Inifile->value("NDT_transepsilon").toDouble();
  NDT_stepsize = Inifile->value("NDT_stepsize").toDouble();
  NDT_resolution = Inifile->value("NDT_resolution").toFloat();
  NDT_maxiteration = Inifile->value("NDT_maxiteration").toInt();

  SACIA_number_samples = Inifile->value("SACIA_number_samples").toInt();
  SACIA_randomness = Inifile->value("SACIA_randomness").toInt();
  SACIA_min_sample_distance =
      Inifile->value("SACIA_min_sample_distance").toFloat();
  SACIA_max_correspondence_distance =
      Inifile->value("SACIA_max_correspondence_distance").toDouble();
  SACIA_max_iterations = Inifile->value("SACIA_max_iterations").toInt();

  ICP_max_corr_distance = Inifile->value("ICP_max_corr_distance").toDouble();
  ICP_max_iter_icp = Inifile->value("ICP_max_iter_icp").toInt();
  ICP_transformation = Inifile->value("ICP_transformation").toDouble();
  ICP_euclidean_Fitness = Inifile->value("ICP_euclidean_Fitness").toDouble();

  std::cout << "ICP_euclidean_Fitness : " << ICP_euclidean_Fitness << std::endl;
  delete Inifile;
  return true;
}