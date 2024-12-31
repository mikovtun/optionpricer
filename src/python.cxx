#include <pybind11/pybind11.h>
#include <string>
#include <map>
#include "stock.h"
#include "option.h"


namespace py = pybind11;

namespace OP {
  
 const std::map<Device, std::string> DeviceNames = {
  {Device::cpu, "_cpu"},
  {Device::gpu, "_gpu"}
};


// Templated helper function to expose LN
template <Device d>
void expose_LN(py::module_ &m) {
    std::string class_name = std::string("LN") + DeviceNames.at(d);
    py::class_<LN<d>, Stock, std::shared_ptr<LN<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float>())  
        .def("pointer", &LN<d>::pointer,                      py::return_value_policy::reference)  
        .def_readwrite("bias", &LN<d>::bias)  
        .def_readwrite("volatility", &LN<d>::volatility);  
}

// Templated helper function to expose LNDividend
template <Device d>
void expose_LNDividend(py::module_ &m) {
    std::string class_name = std::string("LNDividend") + DeviceNames.at(d);
    py::class_<LNDividend<d>, Stock, std::shared_ptr<LNDividend<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float, float>())  
        .def("pointer", &LNDividend<d>::pointer,                      py::return_value_policy::reference)  
				.def_readwrite("dividendYield", &LNDividend<d>::dividendYield);
}

// Templated helper function to expose LNDiscreteDividend
template <Device d>
void expose_LNDiscreteDividend(py::module_ &m) {
    std::string class_name = std::string("LNDiscreteDividend") + DeviceNames.at(d);
    py::class_<LNDiscreteDividend<d>, Stock, std::shared_ptr<LNDiscreteDividend<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float, float, float, float>())  
        .def("pointer", &LNDiscreteDividend<d>::pointer,                      py::return_value_policy::reference)  
				.def_readwrite("dividendYield", &LNDiscreteDividend<d>::dividendYield)
				.def_readwrite("daysTillFirstDividend", &LNDiscreteDividend<d>::daysTillFirstDividend)
				.def_readwrite("dividendInterval", &LNDiscreteDividend<d>::dividendInterval);
}



PYBIND11_MODULE(optionpricer, m) {
  py::enum_<Device>(m, "Device")
    .value("cpu", Device::cpu)
    .value("gpu", Device::gpu)
    .export_values();


  py::class_<Stock, std::shared_ptr<Stock>>(m, "Stock")
    .def("getPrices", &Stock::getPrices)
    .def_readwrite("start", &Stock::start);

	expose_LN<Device::cpu>(m);
	expose_LN<Device::gpu>(m);

	expose_LNDividend<Device::cpu>(m);
	expose_LNDividend<Device::gpu>(m);

	expose_LNDiscreteDividend<Device::cpu>(m);
	expose_LNDiscreteDividend<Device::gpu>(m);


  py::class_<OptionPosition>(m, "OptionPosition")
    .def(py::init<std::shared_ptr<Stock>>())  
    .def("getPrice", &OptionPosition::getPrice, py::arg("accuracy") = 0.01)
    .def("longShares", &OptionPosition::longShares)  
    .def("shortShares", &OptionPosition::shortShares)  
    .def("setShares", &OptionPosition::setShares)  
    .def("longCall", &OptionPosition::longCall)  
    .def("longPut", &OptionPosition::longPut)  
    .def("shortCall", &OptionPosition::shortCall)  
    .def("shortPut", &OptionPosition::shortPut)  
    .def_readwrite("underlying", &OptionPosition::underlying)  
    .def_readwrite("stockPosition", &OptionPosition::stockPosition)  
    .def_readwrite("options", &OptionPosition::options);  

}

}
