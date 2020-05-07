load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "imu_supervisor",
    srcs = ["imu_supervisor.cc"],
    hdrs = ["imu_supervisor.h"],
    deps = [
        "//modules/supervisor/common:supervisor_runner",
        "//modules/drivers/gnss/proto:gnss_proto",
        "//modules/common/adapters:adapter_gflags",
        "//modules/supervisor/proto:supervisor_proto"
    ],
)

cc_library(
    name = "gnss_supervisor",
    srcs = ["gnss_supervisor.cc"],
    hdrs = ["gnss_supervisor.h"],
    deps = [
        "//modules/supervisor/common:supervisor_runner",
        "//modules/drivers/gnss/proto:gnss_proto",
        "//modules/common/adapters:adapter_gflags",
        "//modules/supervisor/proto:supervisor_proto"
    ],
)

cpplint()