load("//tools:py_proto.bzl", "py_proto")

package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "supervisor_proto",
    deps = [
        ":supervisor_proto_lib",
    ],
)

proto_library(
    name = "supervisor_proto_lib",
    srcs = [
        "general_msg.proto",
    ],
    deps = [
        "//modules/common/proto:header_proto_lib",
    ],
)