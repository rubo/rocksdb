//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "memory/mimalloc_allocator.h"

#include "port/mimalloc_helper.h"

namespace ROCKSDB_NAMESPACE {

bool MimallocAllocator::IsSupported(std::string* why) {
#ifndef ROCKSDB_MIMALLOC
  *why = "Not compiled with ROCKSDB_MIMALLOC";
  return false;
#else
  if (!HasMimalloc()) {
    *why = "Mimalloc is not linked with the binary";
    return false;
  }
  return true;
#endif  // ROCKSDB_MIMALLOC
}

#ifdef ROCKSDB_MIMALLOC
void* MimallocAllocator::Allocate(size_t size) { return mi_malloc(size); }

void MimallocAllocator::Deallocate(void* p) { mi_free(p); }

size_t MimallocAllocator::UsableSize(void* p,
                                     size_t /*allocation_size*/) const {
  return mi_usable_size(p);
}
#endif  // ROCKSDB_MIMALLOC

Status NewMimallocAllocator(
    std::shared_ptr<MemoryAllocator>* memory_allocator) {
  if (memory_allocator == nullptr) {
    return Status::InvalidArgument("memory_allocator must be non-null.");
  }
#ifndef ROCKSDB_MIMALLOC
  return Status::NotSupported("Not compiled with MIMALLOC");
#else
  memory_allocator->reset(new MimallocAllocator());
  return Status::OK();
#endif  // ROCKSDB_MIMALLOC
}

}  // namespace ROCKSDB_NAMESPACE
