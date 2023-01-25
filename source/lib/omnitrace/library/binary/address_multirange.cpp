// MIT License
//
// Copyright (c) 2022 Advanced Micro Devices, Inc. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "library/binary/address_multirange.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

namespace omnitrace
{
namespace binary
{
address_multirange&
address_multirange::operator+=(std::pair<coarse, uintptr_t>&& _v)
{
    coarse_range = address_range{ std::min(coarse_range.low, _v.second),
                                  std::max(coarse_range.high, _v.second) };
    return *this;
}

address_multirange&
address_multirange::operator+=(std::pair<coarse, address_range>&& _v)
{
    coarse_range = address_range{ std::min(coarse_range.low, _v.second.low),
                                  std::max(coarse_range.high, _v.second.high) };

    return *this;
}

address_multirange&
address_multirange::operator+=(uintptr_t _v)
{
    *this += std::make_pair(coarse{}, _v);

    for(auto&& itr : m_fine_ranges)
        if(itr.contains(_v)) return *this;

    m_fine_ranges.emplace(address_range{ _v });
    return *this;
}

address_multirange&
address_multirange::operator+=(address_range _v)
{
    *this += std::make_pair(coarse{}, _v);

    for(auto&& itr : m_fine_ranges)
        if(itr.contains(_v)) return *this;

    m_fine_ranges.emplace(_v);
    return *this;
}
}  // namespace binary
}  // namespace omnitrace
