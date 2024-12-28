#include "stock.h"


namespace OP {
  template class LogNormalStock<Device::cpu>;
  template class LogNormalStock<Device::gpu>;
  template class LogNormalStockDividend<Device::cpu>;
  template class LogNormalStockDividend<Device::gpu>;
  template class LogNormalStockDiscreteDividend<Device::cpu>;
  template class LogNormalStockDiscreteDividend<Device::gpu>;
}
