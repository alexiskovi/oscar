#include "modules/supervisor/software/gnss_supervisor.h"

DEFINE_double(gnss_check_period, 0.2,
            "Interval in seconds.");
DEFINE_double(gnss_timeout_sec, 2.0,
            "Timeout in seconds.");            
DEFINE_string(sv_gnss_conf_file, "/apollo/modules/supervisor/conf/gnss_conf.pb.txt",
            "Path to GNSS conf file");

std::vector <apollo::drivers::gnss::SolutionType> sol_types_allowed = {
  apollo::drivers::gnss::SolutionType::FIXEDPOS,
  apollo::drivers::gnss::SolutionType::FIXEDHEIGHT,
  apollo::drivers::gnss::SolutionType::NARROW_INT,
  apollo::drivers::gnss::SolutionType::NARROW_FLOAT,
  apollo::drivers::gnss::SolutionType::PSRDIFF,
};

namespace apollo {
namespace supervisor {

struct compare {
  int key;
  compare(int const &i): key(i) {}
  bool operator()(int const &i){
    return (i == key);
  }
};

GNSSSupervisor::GNSSSupervisor() 
    : SupervisorRunner("GNSSSupervisor", FLAGS_gnss_check_period) {
  sv_sub_node_ = apollo::cyber::CreateNode("sv_gnss_sub_node");
  best_pose_reader_ = sv_sub_node_->CreateReader<apollo::drivers::gnss::GnssBestPose>("/apollo/sensor/gnss/best_pose", nullptr);
  ACHECK(best_pose_reader_ != nullptr);
  ACHECK(
    cyber::common::GetProtoFromFile(FLAGS_sv_gnss_conf_file, &gnss_conf_))
    << "Unable to load gnss conf file: " + FLAGS_sv_gnss_conf_file;
}

void GNSSSupervisor::RunOnce(const double current_time) {
  status_ = 0;

  best_pose_reader_->Observe();
  const auto &best_pose_msg = best_pose_reader_->GetLatestObserved();

  if(best_pose_msg == nullptr) {
    AERROR << "GNSS message is not ready";
    return void();
  }
  // Check timeout
  if(current_time - best_pose_msg->header().timestamp_sec() > FLAGS_gnss_timeout_sec){
    status_ = std::max((int)gnss_conf_.timeout_priority(), status_);
    debug_msg_ = "GNSS message timeout";
  }
  // Check differential age
  if(best_pose_msg->differential_age() > gnss_conf_.max_differential_age()){
    status_ = std::max((int)gnss_conf_.differential_age_timeout_priority(), status_);
    debug_msg_ = "GNSS differential age timeout";
  }
  // Check solution status
  if(best_pose_msg->sol_status() != apollo::drivers::gnss::SolutionStatus::SOL_COMPUTED){
    status_ = std::max((int)gnss_conf_.bad_sol_status_priority(), status_);
    debug_msg_ = "GNSS invalid solution status";
  }
  // Check solution type
  if(!std::any_of(sol_types_allowed.begin(), sol_types_allowed.end(), compare(best_pose_msg->sol_type()))){
    status_ = std::max((int)gnss_conf_.bad_sol_type_priority(), status_);
    debug_msg_ = "Current GNSS solution type not in white-list";
  }
  // Check standart deviation
  double lat_std_dev = best_pose_msg->latitude_std_dev();
  double lon_std_dev = best_pose_msg->longitude_std_dev();
  double linear_dev = std::sqrt(lat_std_dev * lat_std_dev + lon_std_dev * lon_std_dev);
  if(linear_dev > gnss_conf_.max_deviation_allowed()){
    status_ = std::max((int)gnss_conf_.bad_deviation_priority(), status_);
    debug_msg_ = "Standart GNSS deviation is above threshold";
  }
}

void GNSSSupervisor::GetStatus(int* status, std::string* debug_msg) {
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}