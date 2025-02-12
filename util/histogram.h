//
// Copyright 2021 Splunk Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef INCLUDE_FASTPASS_UTIL_HISTOGRAM_H_
#define INCLUDE_FASTPASS_UTIL_HISTOGRAM_H_

/**
 * @returns the bin the item should go into, if the histogram has n_bins
 * @important: n_bins must be a power of 2
 */
static inline __attribute__((always_inline)) unsigned int
histogram_bin(unsigned int n_bins, int item)
{
  unsigned int mask = n_bins - 1;
  /* if item is above h->mask, overflow mask will be ~0, otherwise 0 */
  unsigned int overflow_mask = ((int)(mask)-item) >> 31;
  /* if item is negative, shifting right will produce ~0, otherwise 0 */
  unsigned int neg_mask = (item >> 31);
  /* if overflow, clasped will be ~0, if negative, clasp will be 0 */
  unsigned int clasped = (((unsigned int)item) & (~neg_mask)) | overflow_mask;
  /* increment bin */
  return (clasped & mask);
}

#endif /* INCLUDE_FASTPASS_UTIL_HISTOGRAM_H_ */
