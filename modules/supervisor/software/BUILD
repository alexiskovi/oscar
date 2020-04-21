load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "imu_supervisor",
    srcs = ["imu_supervisor.cc"],
    hdrs = ["imu_supervisor.h"],
    deps = [
        "//modules/supervisor/common:supervisor_runner",
    ],
)

cpplint()