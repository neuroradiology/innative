/* inNative Runtime Compiler
Copyright (c)2019 Black Sphere Studios

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __INNATIVE_H__IN__
#define __INNATIVE_H__IN__

#define INNATIVE_VERSION_MAJOR 0
#define INNATIVE_VERSION_MINOR 1
#define INNATIVE_VERSION_REVISION 0
#define INNATIVE_VERSION(v, m, r, b) (((v|0ULL)<<48) | ((m|0ULL)<<32) | ((r|0ULL)<<16) | (b|0ULL))

// CPU Architecture (possible pre-defined macros found on http://predef.sourceforge.net/prearch.html)
#if defined(_M_X64) || defined(__amd64__) || defined(__amd64) || defined(_AMD64_) || defined(__x86_64__) || defined(__x86_64) || defined(_LP64)
#define IN_CPU_x86_64  //x86-64 architecture
#define IN_64BIT
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64)
#define IN_CPU_IA_64 //Itanium (IA-64) architecture
#define IN_64BIT
#elif defined(_M_IX86) || defined(__i386) || defined(__i386__) || defined(__X86__) || defined(_X86_) || defined(__I86__) || defined(__THW_INTEL__) || defined(__INTEL__)
#define IN_CPU_x86  //x86 architecture
#define IN_32BIT
#elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM)
#define IN_CPU_ARM //ARM architecture
#define IN_32BIT
#elif defined(__mips__) || defined(mips) || defined(_MIPS_ISA) || defined(__mips) || defined(__MIPS__)
#define IN_CPU_MIPS
#define IN_64BIT
#elif defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(_ARCH_PPC)
#define IN_CPU_POWERPC
#define IN_32BIT
#else
#define IN_CPU_UNKNOWN //Unknown CPU architecture (should force architecture independent C implementations)
#endif

// Compiler detection and macro generation
#if defined(__clang__) // Clang (must be before GCC, because clang also pretends it's GCC)
#define IN_COMPILER_CLANG
#define IN_COMPILER_DLLEXPORT __attribute__((dllexport))
#define IN_COMPILER_DLLIMPORT __attribute__((dllimport))
#define IN_COMPILER_FASTCALL __attribute__((fastcall))
#define IN_COMPILER_NAKED __attribute__((naked))
#define IN_FORCEINLINE __attribute__((always_inline)) inline
#define IN_RESTRICT __restrict__
#define IN_ALIGN(n) __attribute__((aligned(n)))
#define IN_ALIGNED(sn, n) sn IN_ALIGN(n)
#elif defined __GNUC__ // GCC
#define IN_COMPILER_GCC
#define IN_COMPILER_DLLEXPORT __attribute__((dllexport))
#define IN_COMPILER_DLLIMPORT __attribute__((dllimport))
#define IN_COMPILER_FASTCALL __attribute__((fastcall))
#define IN_COMPILER_NAKED __attribute__((naked))
#define IN_FORCEINLINE __attribute__((always_inline)) inline
#define IN_RESTRICT __restrict__
#define IN_ALIGN(n) __attribute__((aligned(n)))
#define IN_ALIGNED(sn, n) sn IN_ALIGN(n)
#elif defined _MSC_VER // VC++
#define IN_COMPILER_MSC
#define IN_COMPILER_DLLEXPORT __declspec(dllexport)
#define IN_COMPILER_DLLIMPORT __declspec(dllimport)
#define IN_COMPILER_FASTCALL __fastcall
#define IN_FORCEINLINE __forceinline
#define IN_RESTRICT __restrict
#define IN_ALIGN(n) __declspec(align(n))
#define IN_ALIGNED(sn, n) IN_ALIGN(n) sn
#define IN_SSE_ENABLED
#define IN_ASSUME(x) __assume(x)
#define _HAS_EXCEPTIONS 0
#endif

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(_WIN64) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#define IN_PLATFORM_WIN32
#elif defined(_POSIX_VERSION) || defined(_XOPEN_VERSION) || defined(unix) || defined(__unix__) || defined(__unix)
#define IN_PLATFORM_POSIX
#endif

#ifdef _WIN32_WCE
#define IN_PLATFORM_WIN32_CE // Implies WIN32
#elif defined(__APPLE__) || defined(__MACH__) || defined(macintosh) || defined(Macintosh)
#define IN_PLATFORM_APPLE // Should also define POSIX, use only for Apple OS specific features
#elif defined(__CYGWIN__)
#define IN_PLATFORM_CYGWIN // Should also define POSIX, use only to deal with Cygwin weirdness
#elif defined(__ANDROID__) || defined(__ANDROID_API__) 
#define IN_PLATFORM_ANDROID // Should also define POSIX, use for Android specific features.
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__) || defined(BSD) // Also defines POSIX
#define IN_PLATFORM_BSD // Should also define POSIX
#elif defined(sun) || defined(__sun) 
# define IN_PLATFORM_SOLARIS
# if !defined(__SVR4) && !defined(__svr4__)
#   define IN_PLATFORM_SUNOS
# endif
#endif

#if defined(__linux__) || defined(__linux)
#define IN_PLATFORM_LINUX // Should also define POSIX, use only for linux specific features
#endif

#if !(defined(IN_PLATFORM_WIN32) || defined(IN_PLATFORM_POSIX) || defined(IN_PLATFORM_WIN32_CE) || defined(IN_PLATFORM_APPLE))
#error "Unknown Platform"
#endif

// Endianness detection
#if defined(IN_PLATFORM_WIN32) || defined(IN_PLATFORM_WIN32_CE) || defined(IN_CPU_x86_64) || defined(IN_CPU_x86) || defined(IN_CPU_IA_64) // Windows, x86, x86_64 and itanium all only run in little-endian (except on HP-UX but we don't support that)
# define IN_ENDIAN_LITTLE
#elif defined(IN_CPU_ARM)
# ifdef IN_PLATFORM_LINUX
#   define IN_ENDIAN_LITTLE
# endif
#elif defined(IN_CPU_POWERPC)
# ifdef IN_PLATFORM_SOLARIS
#   define IN_ENDIAN_LITTLE
# elif defined(IN_PLATFORM_APPLE) || defined(IN_PLATFORM_BSD) || defined(IN_PLATFORM_LINUX)
#   define IN_ENDIAN_BIG
# endif
#elif defined(IN_CPU_MIPS) // MIPS is a bitch to detect endianness on
# ifdef IN_PLATFORM_LINUX
#   define IN_ENDIAN_BIG
# endif
#endif

#if !defined(IN_ENDIAN_LITTLE) && !defined(IN_ENDIAN_BIG)
#error "Unknown Endianness"
#endif

// Debug detection
#ifdef IN_COMPILER_GCC
#ifndef NDEBUG
#define IN_DEBUG
#endif
#else
#if defined(DEBUG) || defined(_DEBUG)
#define IN_DEBUG
#endif
#endif

#ifdef IN_COMPILER_MSC
#define FOPEN(f, path, mode) fopen_s((&f), (path), (mode))
#define STRICMP(a, b) _stricmp(a, b)
#define STRTOK(str,delim,context) strtok_s(str,delim,context)
#define FPRINTF(f,format,...) fprintf_s(f, format, __VA_ARGS__)
#define SPRINTF(b, c, f, ...) sprintf_s(b, c, f, __VA_ARGS__)
#define UNLINK(s) _unlink(s)
#else
#define FOPEN(f, path, mode) f = fopen(path, mode)
#define STRICMP(a, b) strcasecmp(a, b)
#define STRTOK(str,delim,context) strtok_r(str,delim,context)
#define FPRINTF(f,format,...) fprintf(f, format, __VA_ARGS__)
#define SPRINTF(b, c, f, ...) sprintf(b, f, __VA_ARGS__)
#define UNLINK(s) unlink(s)
#endif

#endif
