load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "lexus_rx_vehicle_factory",
    srcs = [
        "lexus_rx_vehicle_factory.cc",
    ],
    hdrs = [
        "lexus_rx_vehicle_factory.h",
    ],
    deps = [
        ":lexus_rx_controller",
        ":lexus_rx_message_manager",
        "//modules/canbus/vehicle:abstract_vehicle_factory",
    ],
)

cc_library(
    name = "lexus_rx_message_manager",
    srcs = [
        "lexus_rx_message_manager.cc",
    ],
    hdrs = [
        "lexus_rx_message_manager.h",
    ],
    deps = [
        "//modules/canbus/proto:canbus_proto",
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "//modules/drivers/canbus/can_comm:message_manager_base",
        "//modules/drivers/canbus/common:canbus_common",
    ],
)

cc_library(
    name = "lexus_rx_controller",
    srcs = [
        "lexus_rx_controller.cc",
        "lexus_rx_steer_controller.cc",
    ],
    hdrs = [
        "lexus_rx_controller.h",
        "lexus_rx_steer_controller.h",
    ],
    deps = [
        ":lexus_rx_message_manager",
        "//modules/canbus/proto:canbus_proto",
        "//modules/canbus/vehicle:vehicle_controller_base",
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "//modules/common/kv_db",
        "//modules/drivers/canbus/can_comm:can_sender",
        "//modules/drivers/canbus/can_comm:message_manager_base",
        "//modules/drivers/canbus/common:canbus_common",
        "//modules/control/common:pid_steer_controller",
    ],
)

cc_test(
    name = "lexus_rx_controller_test",
    size = "small",
    srcs = [
        "lexus_rx_controller_test.cc",
    ],
    data = ["//modules/canbus:canbus_testdata"],
    deps = [
        "//modules/canbus/vehicle/lexus_rx:lexus_rx_controller",
        "@gtest//:main",
    ],
)

cpplint()
