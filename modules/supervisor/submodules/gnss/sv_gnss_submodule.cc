#include "modules/supervisor/submodules/gnss/sv_gnss_submodule.h"

DEFINE_double(gnss_check_period, 0.3,
            "Interval in seconds.");            
DEFINE_string(sv_gnss_conf_file, "/apollo/modules/supervisor/submodules/conf/gnss_conf.pb.txt",
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
  sv_gnss_node_ = apollo::cyber::CreateNode("sv_gnss_node");
  best_pose_reader_ = sv_gnss_node_->CreateReader<apollo::drivers::gnss::GnssBestPose>("/apollo/sensor/gnss/best_pose", nullptr);
  ACHECK(best_pose_reader_ != nullptr);
  ACHECK(
    cyber::common::GetProtoFromFile(FLAGS_sv_gnss_conf_file, &gnss_conf_))
    << "Unable to load gnss conf file: " + FLAGS_sv_gnss_conf_file;
  gnss_status_writer_ = sv_gnss_node_->CreateWriter<sv_gnss_msg>("/supervisor/gnss/status");
}

void GNSSSupervisor::RunOnce(const double current_time) {
  status_ = 0;

  best_pose_reader_->Observe();
  const auto &best_pose_msg = best_pose_reader_->GetLatestObserved();

  if(best_pose_msg == nullptr) {
    AERROR << "GNSS message is not ready";
    status_ = (int)gnss_conf_.not_ready_priority();
    debug_msg_ = "GNSS is not ready. Check that GNSS module is on.";
    return void();
  }
  // Check timeout
  if(current_time - best_pose_msg->header().timestamp_sec() > gnss_conf_.max_timeout_s()){
    if(status_ < (int)gnss_conf_.timeout_priority()){
      status_ = (int)gnss_conf_.timeout_priority();
      debug_msg_ = "GNSS message timeout";
    }
  }
  // Check differential age
  if(best_pose_msg->differential_age() > gnss_conf_.max_differential_age()){
    if(status_ < (int)gnss_conf_.differential_age_timeout_priority()){
      status_ = (int)gnss_conf_.differential_age_timeout_priority();
      debug_msg_ = "GNSS differential age timeout";
    }
  }
  // Check solution status
  if(best_pose_msg->sol_status() != apollo::drivers::gnss::SolutionStatus::SOL_COMPUTED){
    if(status_ < (int)gnss_conf_.bad_sol_status_priority()){
      status_ = (int)gnss_conf_.bad_sol_status_priority();
      debug_msg_ = "GNSS invalid solution status";
    }
  }
  // Check solution type
  if(!std::any_of(sol_types_allowed.begin(), sol_types_allowed.end(), compare(best_pose_msg->sol_type()))){
    if(status_ < (int)gnss_conf_.bad_sol_type_priority()){
      status_ = (int)gnss_conf_.bad_sol_type_priority();
      debug_msg_ = "current GNSS solution type is not in white-list";
    }
  }
  // Check standart deviation
  double lat_std_dev = best_pose_msg->latitude_std_dev();
  double lon_std_dev = best_pose_msg->longitude_std_dev();
  double linear_dev = std::sqrt(lat_std_dev * lat_std_dev + lon_std_dev * lon_std_dev);
  if(linear_dev > gnss_conf_.max_deviation_allowed()){
    if(status_ < (int)gnss_conf_.bad_deviation_priority()){
      status_ = (int)gnss_conf_.bad_deviation_priority();
      debug_msg_ = "Standart GNSS deviation is above threshold";
    }
  }

  // Fill status message
  sv_gnss_msg msg;
  msg.set_differential_age(best_pose_msg->differential_age());
  msg.set_lateral_error(lat_std_dev);
  msg.set_longitudinal_error(lon_std_dev);
  if(status_ < 10) msg.set_overall_status("OK");
  if((status_ >= 10)&&(status_ < 20)) msg.set_overall_status("WARNING");
  if((status_ >= 20)&&(status_ < 30)) msg.set_overall_status("ERROR");
  if((status_ >= 30)&&(status_ < 40)) msg.set_overall_status("FATAL");

  gnss_status_writer_->Write(msg);
}

void GNSSSupervisor::GetStatus(std::string* submodule_name, int* status, std::string* debug_msg) {
  *submodule_name = "gnss";
  *status = status_;
  *debug_msg = debug_msg_;
}

}
}
