#ifdef _WIN32
/* Windows x64/x86 */
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define PLATFORM_WINDOWS

#ifdef _WIN64
	/* Windows x64  */
#define GARBAGE_PLATFORM_WINDOWS
#else
	/* Windows x86 */
#error "x86 Builds are not supported!"
#endif
#elif defined(__linux__)
#define GARBAGE_PLATFORM_LINUX
#error "Linux is not supported! (yet)"
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif

#define COMPILER_CLANG 0
#define COMPILER_GCC 1
#define COMPILER_MSVC 2
#define COMPILER_MINGW 3
#define COMPILER_UNKNOWN -1

#if defined(__clang__) || defined(__llvm__)
#define COMPILER COMPILER_CLANG
#define FORCEINLINE __attribute__((always_inline))
#define GARBAGE_NODISCARD 
#elif defined(__GNUC__)
#define COMPILER COMPILER_GCC
#define FORCEINLINE __attribute__((always_inline))
#define GARBAGE_NODISCARD 
#elif defined(_MSC_VER)
#define COMPILER COMPILER_MSVC
#define FORCEINLINE __forceinline
#define GARBAGE_NODISCARD [[nodiscard]]
#elif defined(__MINGW32__) || defined(__MINGW64__)
#define COMPILER COMPILER_MINGW
#define FORCEINLINE __attribute__((always_inline))
#define GARBAGE_NODISCARD 
#else
#define COMPILER COMPILER_UNKNOWN
#define FORCEINLINE inline
#define GARBAGE_NODISCARD 
//#error "Unsupported compiler!"
#endif