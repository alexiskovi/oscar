load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "gnss_submodule",
    srcs = ["sv_gnss_submodule.cc"],
    hdrs = ["sv_gnss_submodule.h"],
    deps = [
        "//modules/supervisor/common:supervisor_runner",
        "//modules/drivers/gnss/proto:gnss_proto",
        "//modules/common/adapters:adapter_gflags",
        "//modules/supervisor/submodules/proto:sv_submodule_proto"
    ],
)

cpplint()
