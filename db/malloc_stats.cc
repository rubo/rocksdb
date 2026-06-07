//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/malloc_stats.h"

#include <cstring>
#include <memory>

#include "port/jemalloc_helper.h"
#include "port/mimalloc_helper.h"

namespace ROCKSDB_NAMESPACE {

#if defined(ROCKSDB_JEMALLOC) || defined(ROCKSDB_MIMALLOC)

struct MallocStatus {
  char* cur;
  char* end;
};

static void AppendMallocStatus(MallocStatus* mstat, const char* status) {
  size_t status_len = status ? strlen(status) : 0;
  size_t buf_size = (size_t)(mstat->end - mstat->cur);
  if (!status_len || status_len >= buf_size) {
    return;
  }

  snprintf(mstat->cur, buf_size, "%s", status);
  mstat->cur += status_len;
}

#ifdef ROCKSDB_JEMALLOC
static void GetJemallocStatus(void* mstat_arg, const char* status) {
  AppendMallocStatus(static_cast<MallocStatus*>(mstat_arg), status);
}
#endif  // ROCKSDB_JEMALLOC

#ifdef ROCKSDB_MIMALLOC
static void GetMimallocStatus(const char* status, void* mstat_arg) {
  AppendMallocStatus(static_cast<MallocStatus*>(mstat_arg), status);
}
#endif  // ROCKSDB_MIMALLOC

void DumpMallocStats(std::string* stats) {
#ifdef ROCKSDB_JEMALLOC
  if (!HasJemalloc()) {
    return;
  }
#elif defined(ROCKSDB_MIMALLOC)
  if (!HasMimalloc()) {
    return;
  }
#endif
  MallocStatus mstat;
  const unsigned int kMallocStatusLen = 1000000;
  std::unique_ptr<char[]> buf{new char[kMallocStatusLen + 1]};
  mstat.cur = buf.get();
  mstat.end = buf.get() + kMallocStatusLen;
  *mstat.cur = '\0';
#ifdef ROCKSDB_JEMALLOC
  malloc_stats_print(GetJemallocStatus, &mstat, "");
#elif defined(ROCKSDB_MIMALLOC)
  mi_stats_print_out(GetMimallocStatus, &mstat);
#endif
  stats->append(buf.get());
}
#else
void DumpMallocStats(std::string*) {}
#endif  // ROCKSDB_JEMALLOC || ROCKSDB_MIMALLOC
}  // namespace ROCKSDB_NAMESPACE
