load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "canbus_lexus_rx_protocol",
    srcs = [
        "steering_24.cc",
        "steering_25.cc",
        "steering_2e4.cc",
        "steering_260.cc",
        "wheelspeed_aa.cc",
        "speed_b4.cc",
        "rpm_1c4.cc",
        "brake_a6.cc",
        "brake_226.cc",
        "gear_3bc.cc",
        "throttle_343.cc",
        "throttle_245.cc",
        "accel_228.cc",
        "mileage_611.cc",
        "horn_750.cc",
        "steering_262.cc",
        "turnsignal_614.cc",
        "radar_2e6.cc",
        "steering_41.cc",
        "steering_7f0.cc",
        "steering_7f8.cc",
        "acc_1d3.cc",
        "steering_7f1.cc",
        "cruise_361.cc",
        "steering_7fb.cc",
        "gear_777.cc",
    ],
    hdrs = [
        "steering_24.h",
        "steering_25.h",
        "steering_2e4.h",
        "steering_260.h",
        "wheelspeed_aa.h",
        "speed_b4.h",
        "rpm_1c4.h",
        "brake_a6.h",
        "brake_226.h",
        "gear_3bc.h",
        "throttle_343.h",
        "throttle_245.h",
        "accel_228.h",
        "mileage_611.h",
        "horn_750.h",
        "steering_262.h",
        "turnsignal_614.h",
        "radar_2e6.h",
        "steering_41.h",
        "steering_7f0.h",
        "steering_7f8.h",
        "acc_1d3.h",
        "steering_7f1.h",
        "cruise_361.h",
        "steering_7fb.h",
        "gear_777.h",
    ],
    deps = [
        "//modules/canbus/proto:canbus_proto",
        "//modules/drivers/canbus/can_comm:message_manager_base",
        "//modules/drivers/canbus/common:canbus_common",
        "//modules/common/filters:mean_filter",
        "//modules/control/common:pid_controller",
        "//modules/control/common:pid_steer_controller",
    ],
)

cc_test(
    name = "gear_3bc_test",
    size = "small",
    srcs = [
        "gear_3bc_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "throttle_343_test",
    size = "small",
    srcs = [
        "throttle_343_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "throttle_245_test",
    size = "small",
    srcs = [
        "throttle_245_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "accel_228_test",
    size = "small",
    srcs = [
        "accel_228_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "steering_24_test",
    size = "small",
    srcs = [
        "steering_24_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "steering_25_test",
    size = "small",
    srcs = [
        "steering_25_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "steering_260_test",
    size = "small",
    srcs = [
        "steering_260_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "steering_2e4_test",
    size = "small",
    srcs = [
        "steering_2e4_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "rpm_1c4_test",
    size = "small",
    srcs = [
        "rpm_1c4_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "speed_b4_test",
    size = "small",
    srcs = [
        "speed_b4_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "wheelspeed_aa_test",
    size = "small",
    srcs = [
        "wheelspeed_aa_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "brake_a6_test",
    size = "small",
    srcs = [
        "brake_a6_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "brake_226_test",
    size = "small",
    srcs = [
        "brake_226_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "mileage_611_test",
    size = "small",
    srcs = [
        "mileage_611_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "horn_750_test",
    size = "small",
    srcs = [
        "horn_750_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cc_test(
    name = "radar_2e6_test",
    size = "small",
    srcs = [
        "radar_2e6_test.cc",
    ],
    deps = [
        "//modules/canbus/vehicle/lexus_rx/protocol:canbus_lexus_rx_protocol",
        "@gtest//:main",
    ],
)

cpplint()

