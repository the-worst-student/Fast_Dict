#include <pybind11/pybind11.h>
#include "splay_tree.h"

namespace py = pybind11;

using FastDictImpl = SplayTree<py::object, py::object>;

PYBIND11_MODULE(fast_dict, m) {
  py::class_<FastDictImpl>(m, "FastDict")
      .def(py::init<>())

      .def("__getitem__", [](FastDictImpl& self, const py::object& key) {
        try {
          return self.Get(key);
        } catch (const std::out_of_range&) {
          throw py::key_error("Key does not exist");
        }
      })

      .def("__setitem__",
           [](FastDictImpl& self,
              const py::object& key,
              const py::object& value) {
             self.Insert(key, value);
           })

      .def("__delitem__", [](FastDictImpl& self, const py::object& key) {
        if (!self.Contains(key)) {
          throw py::key_error("Key does not exist");
        }
        self.Erase(key);
      })

      .def("__len__", &FastDictImpl::Size)

      .def("__contains__", [](FastDictImpl& self, const py::object& key) {
        return self.Contains(key);
      })

      .def("keys", [](const FastDictImpl& self) {
        py::list result;
        for (const auto& key : self.Keys()) {
          result.append(key);
        }
        return result;
      })

      .def("values", [](const FastDictImpl& self) {
        py::list result;
        for (const auto& value : self.Values()) {
          result.append(value);
        }
        return result;
      })

      .def("items", [](const FastDictImpl& self) {
        py::list result;
        for (const auto& item : self.Items()) {
          result.append(py::make_tuple(item.first, item.second));
        }
        return result;
      })

      .def("__iter__", [](const FastDictImpl& self) {
        py::list key_list;
        for (const auto& key : self.Keys()) {
          key_list.append(key);
        }
        return py::iter(key_list);
      });
}