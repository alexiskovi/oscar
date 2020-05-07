load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sv_guardian_lib",
    srcs = ["sv_guardian_component.cc"],
    hdrs = ["sv_guardian_component.h"],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/supervisor/proto:supervisor_proto",
    ],
)

cc_binary(
    name = "libsv_guardian.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":sv_guardian_lib",
    ],
)

cc_library(
    name = "sv_ipc_lib",
    srcs = ["sv_ipc_component.cc"],
    hdrs = ["sv_ipc_component.h"],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/supervisor/proto:supervisor_proto",
        "//modules/supervisor/software:imu_supervisor",
        "//modules/supervisor/software:gnss_supervisor",
    ],
)

cc_binary(
    name = "libsv_ipc.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":sv_ipc_lib",
    ],
)

cpplint()