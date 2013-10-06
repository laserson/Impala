// Copyright 2012 Cloudera Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef IMPALA_EXPRS_AGGREGATE_FUNCTIONS_H
#define IMPALA_EXPRS_AGGREGATE_FUNCTIONS_H

#include "exprs/opcode-registry.h"
#include "udf/udf.h"

using namespace impala_udf;

namespace impala {

// Collection of builtin aggregate functions. Aggregate functions implement
// the various phases of the aggregation: Init(), Update(), Serialize(), Merge(),
// and Finalize(). Not all functions need to implement all of the steps and
// some of the parts can be reused across different aggregate functions.
// This functions are implemented using the UDA interface.
class AggregateFunctions {
 public:
  // Initializes dst to NULL.
  static void InitNull(FunctionContext*, AnyVal* dst);

  // Initializes dst to 0.
  template <typename T>
  static void InitZero(FunctionContext*, T* dst);

  // Implementation of Count and Count(*)
  static void CountUpdate(FunctionContext*, const AnyVal& src, BigIntVal* dst);
  static void CountStarUpdate(FunctionContext*, BigIntVal* dst);

  // SumUpdate, SumMerge
  template <typename SRC_VAL, typename DST_VAL>
  static void Sum(FunctionContext*, const SRC_VAL& src, DST_VAL* dst);

  // MinUpdate/MinMerge
  template <typename T>
  static void Min(FunctionContext*, const T& src, T* dst);

  // MaxUpdate/MaxMerge
  template <typename T>
  static void Max(FunctionContext*, const T& src, T* dst);

  // For string intermediates, we use the input as a ptr to a scratch struct.
  // We want to convert it to the real string val.
  // TODO: when the planner is updated, input should be a fixed array and not
  // a StringVal.
  static void InitScratch(FunctionContext* c, StringVal* dst);
  static StringVal SerializeScratch(FunctionContext* c, const StringVal& input);

  // String concat
  static void StringConcat(FunctionContext*, const StringVal& src,
      const StringVal& separator, StringVal* result);

  // Probabilistic Counting (PC), a distinct estimate algorithms.
  // Probabilistic Counting with Stochastic Averaging (PCSA) is a variant
  // of PC that runs faster and usually gets equally accurate results.
  static void PcInit(FunctionContext*, StringVal* slot);

  template <typename T>
  static void PcUpdate(FunctionContext*, const T& src, StringVal* dst);
  template <typename T>
  static void PcsaUpdate(FunctionContext*, const T& src, StringVal* dst);

  static void PcMerge(FunctionContext*, const StringVal& src, StringVal* dst);
  static StringVal PcFinalize(FunctionContext*, const StringVal& src);
  static StringVal PcsaFinalize(FunctionContext*, const StringVal& src);
};

}

#endif
