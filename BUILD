cc_binary(
    name = "checker",
    srcs = ["main.cpp"],
    copts = [
        "-std=c++14",
        "-fno-exceptions",
        "-fno-rtti",
    ],
    defines = [
        "__STDC_LIMIT_MACROS",
        "__STDC_CONSTANT_MACROS",
        "__STDC_FORMAT_MACROS",
    ],
    data = [
        "assets/assignment.cpp",
        "assets/compile_commands.json",
    ],
    deps = ["@llvm-project//clang:libclang_static"],
)
