//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).

#include "memory/tcmalloc_allocator.h"

#include "port/tcmalloc_helper.h"

namespace ROCKSDB_NAMESPACE {

bool TCMallocAllocator::IsSupported(std::string* why) {
#ifndef ROCKSDB_TCMALLOC
  *why = "Not compiled with ROCKSDB_TCMALLOC";
  return false;
#else
  if (!HasTCMalloc()) {
    *why = "TCMalloc is not linked with the binary";
    return false;
  }
  return true;
#endif  // ROCKSDB_TCMALLOC
}

#ifdef ROCKSDB_TCMALLOC
void* TCMallocAllocator::Allocate(size_t size) { return tc_malloc(size); }

void TCMallocAllocator::Deallocate(void* p) { tc_free(p); }

size_t TCMallocAllocator::UsableSize(void* p,
                                     size_t /*allocation_size*/) const {
  return tc_malloc_size(p);
}
#endif  // ROCKSDB_TCMALLOC

Status NewTCMallocAllocator(
    std::shared_ptr<MemoryAllocator>* memory_allocator) {
  if (memory_allocator == nullptr) {
    return Status::InvalidArgument("memory_allocator must be non-null.");
  }
#ifndef ROCKSDB_TCMALLOC
  return Status::NotSupported("Not compiled with TCMALLOC");
#else
  memory_allocator->reset(new TCMallocAllocator());
  return Status::OK();
#endif  // ROCKSDB_TCMALLOC
}

}  // namespace ROCKSDB_NAMESPACE
