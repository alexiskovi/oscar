load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sigma_can_client",
    srcs = [
        "sigma_can_client.cc",
        "slmessage/SLPMessage.cc",
    ],
    hdrs = [
        "sigma_can_client.h",
        "slmessage/SLPMessage.h",
        "slmessage/SigmaProtocol.h",
        "serial/serial.h",
        "serial/v8stdint.h",
    ],
    deps = [
        "//modules/drivers/canbus/can_client",
        "//third_party/can_card_library/serial",
    ],
)

cc_test(
    name = "sigma_can_client_test",
    size = "small",
    srcs = [
        "sigma_can_client_test.cc",
    ],
    deps = [
        "//cyber",
        "//modules/drivers/canbus/can_client/sigma:sigma_can_client",
        "//modules/drivers/canbus/common:canbus_common",
        "//third_party/can_card_library/serial",
        "@gtest//:main",
    ],
)

cpplint()

