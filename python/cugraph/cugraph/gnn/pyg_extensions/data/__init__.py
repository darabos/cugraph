# Copyright (c) 2019-2022, NVIDIA CORPORATION.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from cugraph.utilities.api_tools import experimental_warning_wrapper

from cugraph.gnn.pyg_extensions.data.cugraph_store import EXPERIMENTAL__CuGraphStore
from cugraph.gnn.pyg_extensions.data.cugraph_store import EXPERIMENTAL__CuGraphFeatureStore
from cugraph.gnn.pyg_extensions.data.cugraph_store import EXPERIMENTAL__to_pyg

CuGraphStore = experimental_warning_wrapper(EXPERIMENTAL__CuGraphStore)
CuGraphFeatureStore = experimental_warning_wrapper(EXPERIMENTAL__CuGraphFeatureStore)
to_pyg = experimental_warning_wrapper(EXPERIMENTAL__to_pyg)
