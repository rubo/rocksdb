//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#pragma once

#include "rocksdb/memory_allocator.h"
#include "utilities/memory_allocators.h"

namespace ROCKSDB_NAMESPACE {

class MimallocAllocator : public BaseMemoryAllocator {
 public:
  static const char* kClassName() { return "MimallocAllocator"; }
  const char* Name() const override { return kClassName(); }

  static bool IsSupported() {
    std::string unused;
    return IsSupported(&unused);
  }
  static bool IsSupported(std::string* why);

#ifdef ROCKSDB_MIMALLOC
  void* Allocate(size_t size) override;
  void Deallocate(void* p) override;
  size_t UsableSize(void* p, size_t allocation_size) const override;
#endif  // ROCKSDB_MIMALLOC
};

}  // namespace ROCKSDB_NAMESPACE
