/*
 *  Copyright (c) 2016, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */

#include <fatal/type/benchmark/prefix_tree/prefix_tree.h>

#include <fatal/benchmark/driver.h>

namespace fatal {

CREATE_BENCHMARK(n10_len20,
  s20_00, s20_01, s20_02, s20_03, s20_04,
  s20_05, s20_06, s20_07, s20_08, s20_09
);

} // namespace fatal {
