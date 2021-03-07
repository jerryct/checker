# Build

```
mkdir _build
cd _build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release ..
ninja
```

# Run

```
_build/checker -p assets/compile_commands.json assets/assignment.cpp
clang-apply-replacements --remove-change-desc-files assets/
```

```
bazel run //:checker -- -p assets/compile_commands.json assets/assignment.cpp
```
