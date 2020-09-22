package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "sv_submodule_proto",
    deps = [
        ":sv_submodule_proto_lib",
    ],
)

proto_library(
    name = "sv_submodule_proto_lib",
    srcs = [
        "gnss_conf.proto",
        "sv_gnss_msg.proto",
        "imu_conf.proto",
        "sv_imu_msg.proto",
    ],
    deps = [
        "//modules/common/proto:header_proto_lib",
    ],
)