cc_library(
    name = "clang-cpp",
    includes = ["include"],
    hdrs = glob(["include/clang*/**"]),
    srcs = [
        "lib/libclang-cpp.so",
        "lib/libclang-cpp.so.11",
    ],
    deps = [":llvm"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "llvm",
    includes = ["include"],
    hdrs = glob(["include/llvm*/**"]),
)
