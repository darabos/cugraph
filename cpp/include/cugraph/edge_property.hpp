/*
 * Copyright (c) 2022, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cugraph/utilities/dataframe_buffer.hpp>

#include <raft/handle.hpp>

#include <thrust/iterator/iterator_traits.h>

#include <optional>
#include <type_traits>

namespace cugraph {

namespace detail {

template <typename edge_t, typename ValueIterator>
class edge_property_view_t {
 public:
  using value_type     = typename thrust::iterator_traits<ValueIterator>::value_type;
  using value_iterator = ValueIterator;

  edge_property_view_t() = default;

  edge_property_view_t(std::vector<ValueIterator> const& edge_partition_value_firsts,
                       std::vector<edge_t> const& edge_partition_edge_counts)
    : edge_partition_value_firsts_(edge_partition_value_firsts),
      edge_partition_edge_counts_(edge_partition_edge_counts)
  {
  }

  std::vector<ValueIterator> const& value_firsts() const { return edge_partition_value_firsts_; }

  std::vector<edge_t> const& edge_counts() const { return edge_partition_edge_counts_; }

 private:
  std::vector<ValueIterator> edge_partition_value_firsts_{};
  std::vector<edge_t> edge_partition_edge_counts_{};
};

class edge_dummy_property_view_t {
 public:
  using value_type     = thrust::nullopt_t;
  using value_iterator = void*;
};

}  // namespace detail

template <typename GraphViewType, typename T>
class edge_property_t {
 public:
  using edge_type   = typename GraphViewType::edge_type;
  using value_type  = T;
  using buffer_type = decltype(allocate_dataframe_buffer<T>(size_t{0}, rmm::cuda_stream_view{}));

  edge_property_t(raft::handle_t const& handle) {}

  edge_property_t(raft::handle_t const& handle, GraphViewType const& graph_view)
  {
    buffers_.reserve(graph_view.number_of_local_edge_partitions());
    for (size_t i = 0; i < graph_view.number_of_local_edge_partitions(); ++i) {
      buffers_.push_back(allocate_dataframe_buffer<T>(
        graph_view.local_edge_partition_view(i).number_of_edges(), handle.get_stream()));
    }
  }

  void clear(raft::handle_t const& handle)
  {
    buffers_.clear();
    buffers_.shrink_to_fit();
  }

  auto view() const
  {
    using const_value_iterator = decltype(get_dataframe_buffer_cbegin(buffers_[0]));

    std::vector<const_value_iterator> edge_partition_value_firsts(buffers_.size());
    std::vector<edge_type> edge_partition_edge_counts(buffers_.size());
    for (size_t i = 0; i < edge_partition_value_firsts.size(); ++i) {
      edge_partition_value_firsts[i] = get_dataframe_buffer_cbegin(buffers_[i]);
      edge_partition_edge_counts[i]  = size_dataframe_buffer(buffers_[i]);
    }

    return detail::edge_property_view_t<edge_type, const_value_iterator>(
      edge_partition_value_firsts, edge_partition_edge_counts);
  }

  auto mutable_view()
  {
    using value_iterator = decltype(get_dataframe_buffer_begin(buffers_[0]));

    std::vector<value_iterator> edge_partition_value_firsts(buffers_.size());
    std::vector<edge_type> edge_partition_edge_counts(buffers_.size());
    for (size_t i = 0; i < edge_partition_value_firsts.size(); ++i) {
      edge_partition_value_firsts[i] = get_dataframe_buffer_begin(buffers_[i]);
      edge_partition_edge_counts[i]  = size_dataframe_buffer(buffers_[i]);
    }

    return detail::edge_property_view_t<edge_type, value_iterator>(edge_partition_value_firsts,
                                                                   edge_partition_edge_counts);
  }

 private:
  std::vector<buffer_type> buffers_{};
};

class edge_dummy_property_t {
 public:
  using value_type = thrust::nullopt_t;

  auto view() const { return detail::edge_dummy_property_view_t{}; }
};

}  // namespace cugraph
