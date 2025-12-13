#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "rtree.hpp"

namespace py = pybind11;

// Data sould be in following structure
// Data: (payload, ID)

using Value = std::pair<double, std::uint64_t>;
using RTreeValue = RTree<Value>;

PYBIND11_MODULE(rtree_bindings, m) {
    m.doc() = "RTree bindings";

    py::class_<Rect>(m, "Rect")
        .def(py::init<>())
        .def(py::init<double,double,double,double>())
        .def(py::init<double,double>())
        .def_readwrite("xmin", &Rect::xmin)
        .def_readwrite("ymin", &Rect::ymin)
        .def_readwrite("xmax", &Rect::xmax)
        .def_readwrite("ymax", &Rect::ymax);

    py::class_<RTreeValue>(m, "RTree") 
        .def(py::init<>())

        .def("insert",
             [](RTreeValue& self, const Rect& r,
                double payload, std::uint64_t id) {
                 self.Insert(r, Value{payload, id});
             },
             py::arg("rect"), py::arg("payload"), py::arg("id"))

        .def("search",
             [](const RTreeValue& self, const Rect& query) {
                 return self.Search(query);
             },
             py::arg("rect"))

        .def("delete",
             [](RTreeValue& self, const Rect& r,
                double payload, std::uint64_t id) {
                 return self.Delete(r, Value{payload, id});
             },
             py::arg("rect"), py::arg("payload"), py::arg("id"));
}
