// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// CPU specific code for ppc independent of OS goes here.

#if V8_TARGET_ARCH_PPC || V8_TARGET_ARCH_PPC64

#include "src/codegen/cpu-features.h"

#ifdef __APPLE__
#include <mach/mach.h>
#endif

namespace v8 {
namespace internal {

void CpuFeatures::FlushICache(void* buffer, size_t size) {
#if !defined(USE_SIMULATOR)

  #ifdef __APPLE__ // Alternatively, assembler below can be used.
    sys_icache_invalidate(const_cast<void *>(buffer), size);
  #else

  if (CpuFeatures::IsSupported(ICACHE_SNOOP)) {
    __asm__ __volatile__(
        "sync \n"
        "icbi 0, %0  \n"
        "isync  \n"
        : /* no output */
        : "r"(buffer)
        : "memory");
    return;
  }

  const int kInstrCacheLineSize = CpuFeatures::icache_line_size();
  const int kDataCacheLineSize = CpuFeatures::dcache_line_size();
  intptr_t ic_mask = kInstrCacheLineSize - 1;
  intptr_t dc_mask = kDataCacheLineSize - 1;
  byte* start =
      reinterpret_cast<byte*>(reinterpret_cast<intptr_t>(buffer) & ~dc_mask);
  byte* end = static_cast<byte*>(buffer) + size;
  for (byte* pointer = start; pointer < end; pointer += kDataCacheLineSize) {
    __asm__ __volatile__(
        "dcbf 0, %0  \n"
        : /* no output */
        : "r"(pointer));
  }
  __asm__ __volatile__("sync");

  start =
      reinterpret_cast<byte*>(reinterpret_cast<intptr_t>(buffer) & ~ic_mask);
  for (byte* pointer = start; pointer < end; pointer += kInstrCacheLineSize) {
    __asm__ __volatile__(
        "icbi 0, %0  \n"
        : /* no output */
        : "r"(pointer));
  }
  __asm__ __volatile__("isync");

#endif  // !USE_SIMULATOR
}
}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_PPC || V8_TARGET_ARCH_PPC64
