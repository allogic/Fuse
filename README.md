# Fuse

Fuse is a lightweight, almost dependency-free graphics engine built in pure C11.

```
// TODO: Prefix all enums..
// TODO: Prefix all defines..
// TODO: refactor library..
// TODO: refactor db_ prefix..
```

```
cmake -G "Visual Studio 17 2022" ^
	-A x64 ^
	-DLLVM_ENABLE_PROJECTS="clang;lld" ^
	-DLLVM_BUILD_LLVM_C_DYLIB=OFF ^
	-DLLVM_ENABLE_RTTI=ON ^
	-DLLVM_INCLUDE_TESTS=OFF ^
	-DLLVM_INCLUDE_BENCHMARKS=OFF ^
	..
```