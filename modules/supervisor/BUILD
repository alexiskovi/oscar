load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "supervisor_lib",
    srcs = ["sv_component.cc"],
    hdrs = ["sv_component.h"],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/supervisor/common:supervisor_runner",
        "//modules/supervisor/proto:supervisor_proto",
        "//modules/canbus/proto:canbus_proto",
        "//modules/supervisor/submodules/gnss:gnss_submodule",
        "//modules/supervisor/submodules/imu:imu_submodule",
        "@yaml_cpp//:yaml",
    ],
)

cc_binary(
    name = "sv_lib.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":supervisor_lib",
    ],
)

cc_library(
    name = "supervisor_guard_lib",
    srcs = ["sv_guard_component.cc"],
    hdrs = ["sv_guard_component.h"],
    deps = [
        "//cyber",
        "//modules/common/util:message_util",
        "//modules/supervisor/proto:supervisor_proto",
        "//modules/canbus/proto:canbus_proto",
        "//modules/control/proto:control_proto",
        "//modules/guardian/proto:guardian_proto",
        "@yaml_cpp//:yaml",
    ],
)

cc_binary(
    name = "sv_guard_lib.so",
    linkshared = True,
    linkstatic = False,
    deps = [
        ":supervisor_guard_lib",
    ],
)


cpplint()