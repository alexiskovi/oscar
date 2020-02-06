load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "ouster_json",
    srcs = [
        "src/json_reader.cpp",
        "src/json_value.cpp",
        "src/json_tool.h",
        "src/json_writer.cpp",
        "src/json_valueiterator.hpp",
    ],
    hdrs = [
        "include/allocator.h",
        "include/assertions.h",
        "include/autolink.h",
        "include/config.h",
        "include/json_features.h",
        "include/forwards.h",
        "include/json.h",
        "include/reader.h",
        "include/value.h",
        "include/version.h",
        "include/writer.h",
    ],
)

cpplint()