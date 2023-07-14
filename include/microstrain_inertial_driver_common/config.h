/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parker-Lord Driver Definition File
//
// Copyright (c) 2017, Brian Bingham
// Copyright (c) 2020, Parker Hannifin Corp
//
// This code is licensed under MIT license (see LICENSE file for details)
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MICROSTRAIN_INERTIAL_DRIVER_COMMON_CONFIG_H
#define MICROSTRAIN_INERTIAL_DRIVER_COMMON_CONFIG_H

#include <stddef.h>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

#include "mip/definitions/commands_filter.hpp"

#include "microstrain_inertial_driver_common/utils/ros_compat.h"
#include "microstrain_inertial_driver_common/utils/mip/ros_mip_device_main.h"
#include "microstrain_inertial_driver_common/utils/mip/ros_mip_device_aux.h"
#include "microstrain_inertial_driver_common/utils/mappings/mip_publisher_mapping.h"

namespace microstrain
{

static constexpr auto TF_MODE_OFF = 0;
static constexpr auto TF_MODE_GLOBAL = 1;
static constexpr auto TF_MODE_RELATIVE = 2;

const std::vector<double> DEFAULT_MATRIX = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
const std::vector<double> DEFAULT_VECTOR = { 0.0, 0.0, 0.0 };
const std::vector<double> DEFAULT_QUATERNION = { 0.0, 0.0, 0.0, 0.0 };

/**
 * Contains configuration information for the node, configures the device on startup
 *  This class holds the pointer to the MSCL device, so any communication to the device should be done through this class
 */
class Config
{
public:
  /**
   * \brief Default Constructor
   */
  Config() = default;

  /**
   * \brief Constructs the config object with a reference to the ROS node. The reference will be saved as a member variable for later usage
   * \param node  The ROS node that is constructing this object.
   */
  explicit Config(RosNodeType* node);

  /**
   * \brief Reads configuration, and configures the device
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool configure(RosNodeType* node);

  // Generic config options
  bool debug_;
  bool device_setup_;

  // Connection classes and metadata used to interact with the MIP device
  std::shared_ptr<RosMipDeviceMain> mip_device_;
  std::shared_ptr<RosMipDeviceAux> aux_device_;
  std::shared_ptr<MipPublisherMapping> mip_publisher_mapping_;

  // Reconnect varaibles
  int reconnect_attempts_;
  bool configure_after_reconnect_;

  // Info for converting to the ENU frame
  bool use_enu_frame_;
  tf2::Matrix3x3 t_ned2enu_;
  tf2::Matrix3x3 t_microstrain_vehicle_to_ros_vehicle_transform_;

  // Flag for using device timestamp instead of PC received time
  bool use_device_timestamp_;

  // Flag for using ROS time instead of PC received time. If this is and use_device_timestamp_ is set, this will be preferred when setting message timestamps
  bool use_ros_time_;

  // Whether to enable the hardware odometer through the GPIO pins
  bool enable_hardware_odometer_;

  // Whether to publish the velocity in the vehicle frame
  bool filter_vel_in_vehicle_frame_;

  // FILTER
  double gps_leap_seconds_;
  bool filter_enable_gnss_heading_aiding_;
  bool filter_enable_gnss_pos_vel_aiding_;
  bool filter_enable_altimeter_aiding_;
  bool filter_enable_odometer_aiding_;
  bool filter_enable_magnetometer_aiding_;
  bool filter_enable_external_heading_aiding_;
  bool filter_enable_external_gps_time_update_;
  bool filter_enable_wheeled_vehicle_constraint_;
  bool filter_enable_vertical_gyro_constraint_;
  bool filter_enable_gnss_antenna_cal_;
  bool filter_use_compensated_accel_;
  bool filter_relative_pos_config_;
  std::vector<float> filter_speed_lever_arm_;

  // Frame id configuration
  std::string frame_id_;
  std::string target_frame_id_;
  std::string mount_frame_id_;
  std::string map_frame_id_;
  std::string earth_frame_id_;
  std::string gnss_frame_id_[NUM_GNSS];
  std::string odometer_frame_id_;

  // TF mode and transform configuration
  int32_t tf_mode_;

  // IMU frame offset configuration
  bool publish_mount_to_frame_id_transform_;

  // Configured static transforms
  TransformStampedMsg mount_to_frame_id_transform_;

  // Subscriber settings
  bool subscribe_ext_fix_;
  bool subscribe_ext_vel_ned_;
  bool subscribe_ext_vel_enu_;
  bool subscribe_ext_vel_ecef_;
  bool subscribe_ext_vel_body_;
  bool subscribe_ext_pressure_;
  bool subscribe_ext_pose_;
  bool subscribe_ext_heading_;

  // Topic strings
  std::string external_gps_time_topic_;
  std::string external_speed_topic_;

  // RTK config
  bool rtk_dongle_enable_;

  // Aux port nmea and RTCM config
  bool publish_nmea_;
  bool subscribe_rtcm_;
  std::string rtcm_topic_;

  // Static covariance vectors
  std::vector<double> imu_linear_cov_;
  std::vector<double> imu_angular_cov_;
  std::vector<double> imu_orientation_cov_;
  double imu_pressure_vairance_;

  // Gnss antenna offsets
  std::vector<float> gnss_antenna_offset_[NUM_GNSS];

  // Raw data file parameters
  bool raw_file_enable_;
  bool raw_file_include_support_data_;
  std::ofstream raw_file_;
  std::ofstream raw_file_aux_;


  // NMEA streaming parameters
  bool nmea_message_allow_duplicate_talker_ids_;
  float nmea_max_rate_hz_;

private:
  /**
   * \brief Connects to the inertial device and sets up communication
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if connection was successful and false if connection failed
   */
  bool connectDevice(RosNodeType* node);

  /**
   * \brief Configures the device by reading options from the ROS config and sending them to the device
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool setupDevice(RosNodeType* node);

  /**
   * \brief Configures base settings on the intertial device (descriptor set 0x01)
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool configureBase(RosNodeType* node);

  /**
   * \brief Configures 3dm settings on the intertial device (descriptor set 0x0C)
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool configure3DM(RosNodeType* node);

  /**
   * \brief Configures GNSS settings on the inertial device (descriptor set 0x0E)
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool configureGNSS(RosNodeType* node);

  /**
   * \brief Configures Filter settings on the inertial device (descriptor set 0x0D)
   * \param node  The ROS node that contains configuration information. For ROS1 this is the private node handle ("~")
   * \return true if configuration was successful and false if configuration failed
   */
  bool configureFilter(RosNodeType* node);

  /**
   * \brief Enables or disables a filter aiding measurement and handles if the aiding measurement is not supported by this particular device
   * \param aiding_source  The aiding measurement to enable or disable
   * \param enable Whether or not to enable the aiding measurement
   * \return true if the configuration was successful or unsupported, false if the configuration failed
   */
  bool configureFilterAidingMeasurement(const mip::commands_filter::AidingMeasurementEnable::AidingSource aiding_source, const bool enable);

  /**
   * \brief Configures the heading source on a device and handles if the heading source is not supported by this particular device
   * \param heading_source  The heading source to configure
   * \return true if the configuration was successful or unsupported, false if the configuration failed
   */
  bool configureHeadingSource(const mip::commands_filter::HeadingSource::Source heading_source);

  /**
   * \brief Populates a NMEA message format object with configuration from ROS
   * \param config_node  The ROS node that contains configuration information
   * \param data_rate_key  The key to fetch from the config object for the data rate for this sentence
   * \param talker_id  Talker ID to use for this sentence
   * \param descriptor_set  The descriptor set to stream this NMEA sentence from
   * \param message_id  The type of NMEA message to stream
   * \param formats  List of NMEA Message formats to append to
   * \return true if the object was able to be populated properly, false if the object was not able to be populated
   */
  bool populateNmeaMessageFormat(RosNodeType* config_node, const std::string& data_rate_key, mip::commands_3dm::NmeaMessage::TalkerID talker_id, uint8_t descriptor_set, mip::commands_3dm::NmeaMessage::MessageID message_id, std::vector<mip::commands_3dm::NmeaMessage>* formats);

  // Handle to the ROS node
  RosNodeType* node_;
};  // Config class

}  // namespace microstrain

#endif  // MICROSTRAIN_INERTIAL_DRIVER_COMMON_CONFIG_H
