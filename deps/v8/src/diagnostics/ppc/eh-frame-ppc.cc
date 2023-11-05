// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/codegen/ppc/constants-ppc.h"
#include "src/diagnostics/eh-frame.h"

namespace v8 {
namespace internal {

const int EhFrameConstants::kCodeAlignmentFactor = 4;
// all PPC are 4 bytes instruction
#ifdef V8_TARGET_ARCH_PPC64
const int EhFrameConstants::kDataAlignmentFactor = -8;  // 64-bit always -8
#else
const int EhFrameConstants::kDataAlignmentFactor = -4;
#endif

void EhFrameWriter::RecordRegisterNotModified(const int & code) {
    DCHECK_EQ(writer_state_, InternalState::kInitialized);
  WriteOpcode(EhFrameConstants::DwarfOpcodes::kSameValue);
  WriteULeb128(code);
}

void EhFrameWriter::RecordRegisterNotModified(Register name) {
  int code = RegisterToDwarfCode(name);
  return RecordRegisterNotModified(code);
}

void EhFrameWriter::RecordRegisterFollowsInitialRule(const int & code) {
  DCHECK_EQ(writer_state_, InternalState::kInitialized);
  DCHECK_LE(code, EhFrameConstants::kFollowInitialRuleMask);
  WriteByte((EhFrameConstants::kFollowInitialRuleTag
             << EhFrameConstants::kFollowInitialRuleMaskSize) |
            (code & EhFrameConstants::kFollowInitialRuleMask));
}

void EhFrameWriter::RecordRegisterFollowsInitialRule(Register name) {
  int code = RegisterToDwarfCode(name);
  return RecordRegisterFollowsInitialRule(code);
}

void EhFrameWriter::WriteReturnAddressRegisterCode() {
  WriteULeb128(kLrDwarfCode);
}

void EhFrameWriter::WriteInitialStateInCie() {
  SetBaseAddressRegisterAndOffset(fp, 0);
  RecordRegisterNotModified(kLrDwarfCode);
}

// static
int EhFrameWriter::RegisterToDwarfCode(Register name) {
  switch (name.code()) {
    case kRegCode_fp:
      return kFpDwarfCode;
    case kRegCode_sp:
      return kSpDwarfCode;
    case kRegCode_r0:
      return kR0DwarfCode;
    default:
      UNIMPLEMENTED();
      return -1;
  }
}

#ifdef ENABLE_DISASSEMBLER

// static
const char* EhFrameDisassembler::DwarfRegisterCodeToString(int code) {
  switch (code) {
    case kFpDwarfCode:
      return "fp";
    case kSpDwarfCode:
      return "sp";
    default:
      UNIMPLEMENTED();
      return nullptr;
  }
}

#endif

}  // namespace internal
}  // namespace v8
