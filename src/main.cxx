#include <iostream>
#include <chrono>
#include "device_properties.h"
#include "stats.h"
#include "stock.h"
#include "option.h"

namespace OP {

  GPUProperties* GPUProperties::instance = nullptr;
  constexpr float tradingDaysPerAnnum = 365;

  int main(int argc, char* argv[]) {

    GPUProperties* gpuProps = GPUProperties::get();
    
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

    // Info for piecewise defined stock
    std::vector<float> piece_bias   = {0.0, 0.1, 0.2};
    std::vector<float> piece_vol    = {10.0, 25.0, 10.0};
    std::vector<float> piece_times  = {5.0, 10.0};


    auto c_stock_ln           = LN<Device::cpu>( avg, bias, std);
    auto c_stock_ln_div       = LNDividend<Device::cpu>( avg, bias, std, divvy_percent );
    auto c_stock_ln_disc_div  = LNDiscreteDividend<Device::cpu>( avg, bias, std, divvy_percent, divvy_first_payment, divvy_interval);
    auto c_stock_ln_piece     = LNPiecewise<Device::cpu>( avg, piece_bias, piece_vol, piece_times);

    
    auto g_stock_ln           = LN<Device::gpu>( avg, bias, std);
    auto g_stock_ln_div       = LNDividend<Device::gpu>( avg, bias, std , divvy_percent);
    auto g_stock_ln_disc_div  = LNDiscreteDividend<Device::gpu>( avg, bias, std, divvy_percent, divvy_first_payment, divvy_interval);
    auto g_stock_ln_piece     = LNPiecewise<Device::gpu>( avg, piece_bias, piece_vol, piece_times);
    
     
    std::shared_ptr<Stock> c_stock_ln_ptr           = std::make_shared<LN<Device::cpu>>(c_stock_ln);
    std::shared_ptr<Stock> c_stock_ln_div_ptr       = std::make_shared<LNDividend<Device::cpu>>(c_stock_ln_div);
    std::shared_ptr<Stock> c_stock_ln_disc_div_ptr  = std::make_shared<LNDiscreteDividend<Device::cpu>>(c_stock_ln_disc_div);
    std::shared_ptr<Stock> c_stock_ln_piece_ptr     = std::make_shared<LNPiecewise<Device::cpu>>(c_stock_ln_piece);

    std::shared_ptr<Stock> g_stock_ln_ptr           = std::make_shared<LN<Device::gpu>>(g_stock_ln);
    std::shared_ptr<Stock> g_stock_ln_div_ptr       = std::make_shared<LNDividend<Device::gpu>>(g_stock_ln_div);
    std::shared_ptr<Stock> g_stock_ln_disc_div_ptr  = std::make_shared<LNDiscreteDividend<Device::gpu>>(g_stock_ln_disc_div);
    std::shared_ptr<Stock> g_stock_ln_piece_ptr     = std::make_shared<LNPiecewise<Device::gpu>>(g_stock_ln_piece);
    
    OptionPosition c_opt_ln( c_stock_ln_ptr );
    OptionPosition c_opt_ln_div( c_stock_ln_div_ptr );
    OptionPosition c_opt_ln_disc_div( c_stock_ln_disc_div_ptr );
    OptionPosition c_opt_ln_piece( c_stock_ln_piece_ptr );

    OptionPosition g_opt_ln( g_stock_ln_ptr );
    OptionPosition g_opt_ln_div( g_stock_ln_div_ptr );
    OptionPosition g_opt_ln_disc_div( g_stock_ln_disc_div_ptr );
    OptionPosition g_opt_ln_piece( g_stock_ln_piece_ptr );

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

    std::cout << "CALL VALUE (LN PIECEWISE) CPU: " << c_opt_ln_piece.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN PIECEWISE) GPU: " << g_opt_ln_piece.getPrice(accuracy) << std::endl;

    auto testptr = c_stock_ln.pointer();
    
    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
