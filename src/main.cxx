#include <iostream>
#include <chrono>
#include "device_properties.h"
#include "stats.h"
#include "stock.h"
#include "option.h"

#include <pybind11/pybind11.h>

namespace OP {

  GPUProperties* GPUProperties::instance = nullptr;

  int main(int argc, char* argv[]) {

    GPUProperties* gpuProps = GPUProperties::get();
    // Market info
    constexpr float tradingDaysInYear = 365;
    
    // Stock info
    float avg = 100;
    float bias = 0.0;
    float std = 25;

    // Dividend info
    float divvy_percent = 0.05;
    float divvy_interval = 21;
    float divvy_first_payment = 5;

    // Option info
    float expiration = 45;
    float strike = 80.0;
    float accuracy = 0.01;


    auto c_stock_ln = LogNormalStockDividend<Device::cpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear);
    auto c_stock_ln_div = LogNormalStockDividend<Device::cpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear);
    auto c_stock_ln_disc_div = LogNormalStockDiscreteDividend<Device::cpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear, divvy_first_payment, divvy_interval);

    
    auto g_stock_ln = LogNormalStockDividend<Device::gpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear);
    auto g_stock_ln_div = LogNormalStockDividend<Device::gpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear);
    auto g_stock_ln_disc_div = LogNormalStockDiscreteDividend<Device::gpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear, divvy_first_payment, divvy_interval);
    
     
    std::shared_ptr<Stock> c_stock_ln_ptr           = std::make_shared<LogNormalStock<Device::cpu>>(c_stock_ln);
    std::shared_ptr<Stock> c_stock_ln_div_ptr       = std::make_shared<LogNormalStockDividend<Device::cpu>>(c_stock_ln_div);
    std::shared_ptr<Stock> c_stock_ln_disc_div_ptr  = std::make_shared<LogNormalStockDiscreteDividend<Device::cpu>>(c_stock_ln_disc_div);
    std::shared_ptr<Stock> g_stock_ln_ptr           = std::make_shared<LogNormalStock<Device::gpu>>(g_stock_ln);
    std::shared_ptr<Stock> g_stock_ln_div_ptr       = std::make_shared<LogNormalStockDividend<Device::gpu>>(g_stock_ln_div);
    std::shared_ptr<Stock> g_stock_ln_disc_div_ptr  = std::make_shared<LogNormalStockDiscreteDividend<Device::gpu>>(g_stock_ln_disc_div);
    
    OptionPosition c_opt_ln( c_stock_ln_ptr );
    OptionPosition c_opt_ln_div( c_stock_ln_div_ptr );
    OptionPosition c_opt_ln_disc_div( c_stock_ln_disc_div_ptr );
    OptionPosition g_opt_ln( g_stock_ln_ptr );
    OptionPosition g_opt_ln_div( g_stock_ln_div_ptr );
    OptionPosition g_opt_ln_disc_div( g_stock_ln_disc_div_ptr );

    c_opt_ln.longCall(strike, expiration);
    c_opt_ln_div.longCall(strike, expiration);
    c_opt_ln_disc_div.longCall(strike, expiration);
    g_opt_ln.longCall(strike, expiration);
    g_opt_ln_div.longCall(strike, expiration);
    g_opt_ln_disc_div.longCall(strike, expiration);
    
    std::cout << "CALL VALUE (LN) CPU: " << c_opt_ln.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN) GPU: " << g_opt_ln.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DIV) CPU: " << c_opt_ln_div.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DIV) GPU: " << g_opt_ln_div.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DISC DIV) CPU: " << c_opt_ln_disc_div.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DISC DIV) GPU: " << g_opt_ln_disc_div.getPrice(accuracy) << std::endl;

    auto testptr = c_stock_ln.pointer();
    
    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
