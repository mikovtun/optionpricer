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


// Templated helper function to expose LogNormalStock
template <Device d>
void expose_LogNormalStock(py::module_ &m) {
    std::string class_name = std::string("LogNormalStock") + DeviceNames.at(d);
    py::class_<LogNormalStock<d>, Stock, std::shared_ptr<LogNormalStock<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float>())  
        .def("pointer", &LogNormalStock<d>::pointer,                      py::return_value_policy::reference)  
        .def_readwrite("bias", &LogNormalStock<d>::bias)  
        .def_readwrite("volatility", &LogNormalStock<d>::volatility);  
}

// Templated helper function to expose LogNormalStockDividend
template <Device d>
void expose_LogNormalStockDividend(py::module_ &m) {
    std::string class_name = std::string("LogNormalStockDividend") + DeviceNames.at(d);
    py::class_<LogNormalStockDividend<d>, Stock, std::shared_ptr<LogNormalStockDividend<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float, float>())  
        .def("pointer", &LogNormalStockDividend<d>::pointer,                      py::return_value_policy::reference)  
				.def_readwrite("dividendYield", &LogNormalStockDividend<d>::dividendYield);
}

// Templated helper function to expose LogNormalStockDiscreteDividend
template <Device d>
void expose_LogNormalStockDiscreteDividend(py::module_ &m) {
    std::string class_name = std::string("LogNormalStockDiscreteDividend") + DeviceNames.at(d);
    py::class_<LogNormalStockDiscreteDividend<d>, Stock, std::shared_ptr<LogNormalStockDiscreteDividend<d>>>(m, class_name.c_str())
        .def(py::init<float, float, float, float, float, float>())  
        .def("pointer", &LogNormalStockDiscreteDividend<d>::pointer,                      py::return_value_policy::reference)  
				.def_readwrite("dividendYield", &LogNormalStockDiscreteDividend<d>::dividendYield)
				.def_readwrite("daysTillFirstDividend", &LogNormalStockDiscreteDividend<d>::daysTillFirstDividend)
				.def_readwrite("dividendInterval", &LogNormalStockDiscreteDividend<d>::dividendInterval);
}



PYBIND11_MODULE(optionpricer, m) {
  py::enum_<Device>(m, "Device")
    .value("cpu", Device::cpu)
    .value("gpu", Device::gpu)
    .export_values();


  py::class_<Stock, std::shared_ptr<Stock>>(m, "Stock")
    .def("getPrices", &Stock::getPrices)
    .def_readwrite("start", &Stock::start);

	expose_LogNormalStock<Device::cpu>(m);
	expose_LogNormalStock<Device::gpu>(m);

	expose_LogNormalStockDividend<Device::cpu>(m);
	expose_LogNormalStockDividend<Device::gpu>(m);

	expose_LogNormalStockDiscreteDividend<Device::cpu>(m);
	expose_LogNormalStockDiscreteDividend<Device::gpu>(m);


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
