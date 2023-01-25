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

#pragma once

#include "library/binary/address_range.hpp"
#include "library/binary/fwd.hpp"

#include <timemory/utility/macros.hpp>

#include <cstdint>
#include <utility>

namespace omnitrace
{
namespace binary
{
struct address_multirange
{
    struct coarse
    {};

    TIMEMORY_DEFAULT_OBJECT(address_multirange)

    address_multirange& operator+=(std::pair<coarse, uintptr_t>&&);
    address_multirange& operator+=(std::pair<coarse, address_range>&& _v);
    address_multirange& operator+=(uintptr_t _v);
    address_multirange& operator+=(address_range _v);

    template <typename Tp>
    bool contains(Tp&& _v) const;

    address_range coarse_range = {};

    auto size() const { return m_fine_ranges.size(); }
    auto empty() const { return m_fine_ranges.empty(); }
    auto range_size() const { return coarse_range.size(); }

private:
    std::set<address_range> m_fine_ranges = {};
};

template <typename Tp>
OMNITRACE_INLINE bool
address_multirange::contains(Tp&& _v) const
{
    using type = concepts::unqualified_type_t<Tp>;
    static_assert(std::is_integral<type>::value ||
                      std::is_same<type, address_range>::value,
                  "Error! operator+= supports only integrals or address_ranges");

    if(!coarse_range.contains(_v)) return false;
    return std::any_of(m_fine_ranges.begin(), m_fine_ranges.end(),
                       [_v](auto&& itr) { return itr.contains(_v); });
}
}  // namespace binary
}  // namespace omnitrace
