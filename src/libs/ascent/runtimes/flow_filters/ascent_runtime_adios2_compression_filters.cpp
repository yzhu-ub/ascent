//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) Lawrence Livermore National Security, LLC and other Ascent
// Project developers. See top-level LICENSE AND COPYRIGHT files for dates and
// other details. No copyright assignment is required to contribute to Ascent.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: ascent_runtime_adios2_compression_filters.cpp
///
//-----------------------------------------------------------------------------

#include "ascent_runtime_adios2_compression_filters.hpp"

//-----------------------------------------------------------------------------
// thirdparty includes
//-----------------------------------------------------------------------------

// conduit includes
#include <conduit.hpp>
#include <conduit_blueprint.hpp>
#include <conduit_blueprint_mesh.hpp>
#include <conduit_relay.hpp>

//-----------------------------------------------------------------------------
// ascent includes
//-----------------------------------------------------------------------------
#include <ascent_data_object.hpp>
#include <ascent_logging.hpp>
#include <ascent_runtime_utils.hpp>
#include <ascent_string_utils.hpp>

#include <ascent_runtime_vtkh_utils.hpp>
#include <vtkh/DataSet.hpp>
#include <vtkh/vtkh.hpp>

#include <flow_graph.hpp>
#include <flow_workspace.hpp>

// mpi related includes

#include <mpi.h>

#include <adios2.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <string>

#include <fides/DataSetReader.h>
#include <fides/DataSetWriter.h>

using namespace std;
using namespace conduit;
using namespace flow;

template <class T>
inline std::ostream &operator<<(ostream &os, const vector<T> &v) {
  os << "[";
  auto it = v.begin();
  for (; it != v.end(); ++it)
    os << " " << *it;
  os << "]";
  return os;
}

template <class T> inline ostream &operator<<(ostream &os, const set<T> &s) {
  os << "{";
  auto it = s.begin();
  for (; it != s.end(); ++it)
    os << " " << *it;
  os << "}";
  return os;
}

static fides::io::DataSetAppendWriter *writer = NULL;

//-----------------------------------------------------------------------------
// -- begin ascent:: --
//-----------------------------------------------------------------------------
namespace ascent {

//-----------------------------------------------------------------------------
// -- begin ascent::runtime --
//-----------------------------------------------------------------------------
namespace runtime {

//-----------------------------------------------------------------------------
// -- begin ascent::runtime::filters --
//-----------------------------------------------------------------------------
namespace filters {

//-----------------------------------------------------------------------------
ADIOS2Compression::ADIOS2Compression() : Filter() {}

//-----------------------------------------------------------------------------
ADIOS2Compression::~ADIOS2Compression() {}

//-----------------------------------------------------------------------------
void ADIOS2Compression::declare_interface(Node &i) {
  i["type_name"] = "adios2_comp";
  i["port_names"].append() = "in";
  i["output_port"] = "false";
}

//-----------------------------------------------------------------------------
bool ADIOS2Compression::verify_params(const conduit::Node &params,
                                      conduit::Node &info) {
  bool res = true;
  if (!params.has_child("filename") ||
      !params["filename"].dtype().is_string()) {
    info["errors"].append() = "missing required entry 'filename'";
    res = false;
  }

  if (!params.has_child("engine") || !params["engine"].dtype().is_string()) {
    info["errors"].append() = "missing required entry 'engine'";
    res = false;
  }

  std::string engineType = params["engine"].as_string();
  if (engineType != "BPFile" && engineType != "SST") {
    info["errors"].append() = "unsupported engine type: " + engineType;
    res = false;
  }

  std::string fileName = params["filename"].as_string();
  if (engineType == "SST" && fileName.find("/") != std::string::npos) {
    info["errors"].append() =
        "filename with directory not supported for SST engine";
    res = false;
  }

  return res;
}

//-----------------------------------------------------------------------------
void ADIOS2Compression::execute() {
  ASCENT_INFO("execute");

  std::string engineType = params()["engine"].as_string();
  std::string fileName = params()["filename"].as_string();

  if (writer == NULL)
    writer = new fides::io::DataSetAppendWriter(fileName);

  if (!input(0).check_type<DataObject>()) {
    ASCENT_ERROR("ADIOS2Compression input must be a data object");
  }

  // If fields set, set the WriteFields attribute.
  if (params().has_child("fields")) {
    std::string fields = params()["fields"].as_string();
    if (!fields.empty()) {
      std::vector<std::string> fieldList;

      std::istringstream iss(fields);
      std::copy(std::istream_iterator<std::string>(iss),
                std::istream_iterator<std::string>(),
                std::back_inserter(fieldList));

      writer->SetWriteFields(fieldList);
    }
  }

  DataObject *data_object = input<DataObject>(0);
  std::shared_ptr<conduit::Node> node = data_object->as_node();

  auto aaa = node.get();

  (*aaa)[0]["fields/X/values"].print();

  conduit::Node &values = (*aaa)[0]["fields/X/values"];

  for (index_t i = 0; i < 40; i++) {
    const conduit::Node &val = values[i];
    std::cout << "my_vals[" << i << "] = " << val.as_float64() << std::endl;
  }

  std::shared_ptr<VTKHCollection> collection =
      data_object->as_vtkh_collection();

  std::string topo_name =
      detail::resolve_topology(params(), this->name(), collection);

  vtkh::DataSet &data = collection->dataset_by_topology(topo_name);

  vtkm::cont::PartitionedDataSet pds;
  vtkm::Id numDS = data.GetNumberOfDomains();
  for (vtkm::Id i = 0; i < numDS; i++) {
    vtkm::cont::DataSet ds = data.GetDomain(i);
    std::cout << "Num of points:" << ds.GetNumberOfPoints() << std::endl;
    vtkm::cont::Field ff = ds.GetField("X");

    // should be a point field
    std::cout << "Is point field:" << ff.IsPointField() << std::endl;

    vtkm::cont::UnknownArrayHandle &dd = ff.GetData();

    pds.AppendPartition(data.GetDomain(i));
  }

  std::cout << "Pretend to write" << std::endl;

  // writer->Write(pds, engineType);
}

//-----------------------------------------------------------------------------
}; // namespace filters

//-----------------------------------------------------------------------------
// -- end ascent::runtime::filters --
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
}; // namespace runtime
//-----------------------------------------------------------------------------
// -- end ascent::runtime --
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
}; // namespace ascent
//-----------------------------------------------------------------------------
// -- end ascent:: --
//-----------------------------------------------------------------------------