// Copyright (c) 2017 Sony Corporation. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// -*- coding:utf-8 -*-
/*
 * Copyright (C) 2016 Sony Corporation
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation;
 * the contents of this file is not to be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior written
 * permission of Sony Corporation.
 */

/** BinaryWeightAffine
 */
#ifndef __NBLA_FUNCTION_BINARYWEIGHTAFFINE_HPP__
#define __NBLA_FUNCTION_BINARYWEIGHTAFFINE_HPP__

#include <nbla/cpu.hpp>
#include <nbla/function.hpp>
#include <nbla/function_registry.hpp>

namespace nbla {

NBLA_REGISTER_FUNCTION_HEADER(BinaryWeightAffine, int);

/** Binary weight network version of an affine layer, using
    deterministic quantization to -1 and 1 (with scaling).

    Reference:
        Rastegari, Mohammad, et al. "XNOR-Net: ImageNet Classification Using
        Binary Convolutional Neural Networks." arXiv preprint
        arXiv:1603.05279 (2016).

    NOTES:

    1) if you would like to share weights between some layers, please
    make sure to share the standard, floating value weights (input parameter #2)
    and not the binarized weights (input parameter #3)

    2) Only after a call to forward() the weights and the binary weights are in
       sync, not after a call to backward(). If wanting to store the parameters
       of the network, remember to call forward() once before doing so,
       otherwise the weights and the binary weights will not be in sync.

@f[
{\mathbf y} = {\mathbf A} {\mathbf x} + {\mathbf b}.
@f]

Inputs (\f$B\f$ is base_axis):
- Input N-D array with shape
  (\f$M_0 \times ... \times M_{B-1} \times D_B \times ... \times D_N\f$).
  Dimensions before and after base_axis are flattened as if it is a matrix.
- Weight matrix with shape (\f$(D_B \times ... \times D_N) \times L\f$)
- Binarized weight matrix with shape (\f$(D_B \times ... \times D_N) \times
L\f$)
- (optional) Bias vector (\f$L\f$)

Outputs:
- \f$(B + 1)\f$-D array. (\f$ M_0 \times ... \times M_{B-1} \times L \f$)

@tparam T Data type for computation.
@param base_axis Base axis of BinaryConnectAffine operation. Dimensions up to
base_axis
is treated as sample dimension.

\ingroup FunctionImplGrp
 */
template <typename T> class BinaryWeightAffine : public BaseFunction<int> {
protected:
  int base_axis_;
  shared_ptr<Function> sign_;
  shared_ptr<Function> affine_;

  Size_t w_row_, w_col_;

public:
  BinaryWeightAffine(const Context &ctx, int base_axis)
      : BaseFunction(ctx, base_axis), base_axis_(base_axis) {}
  virtual ~BinaryWeightAffine() {}
  virtual shared_ptr<Function> copy() const {
    return create_BinaryWeightAffine(ctx_, base_axis_);
  }
  virtual vector<dtypes> in_types() {
    return vector<dtypes>{get_dtype<T>(), get_dtype<T>(), get_dtype<T>(),
                          get_dtype<T>()};
  }
  virtual vector<dtypes> out_types() { return vector<dtypes>{get_dtype<T>()}; }
  virtual int min_inputs() { return 4; }
  virtual int min_outputs() { return 1; }
  virtual string name() { return "BinaryWeightAffine"; }
  virtual vector<string> allowed_array_classes() {
    return SingletonManager::get<Cpu>()->array_classes();
  }

protected:
  NBLA_API virtual void setup_impl(const Variables &inputs,
                                   const Variables &outputs);
  NBLA_API virtual void forward_impl(const Variables &inputs,
                                     const Variables &outputs);
  NBLA_API virtual void backward_impl(const Variables &inputs,
                                      const Variables &outputs,
                                      const vector<bool> &propagate_down,
                                      const vector<bool> &accum);
};
}
#endif
