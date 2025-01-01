#include <iostream>
#include <chrono>
#include "device_properties.h"
#include "stats.h"
#include "stock.h"
#include "option.h"
#include "defines.h"

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
    float expiration = 5;
    float strike = 80.0;
    float accuracy = 0.01;

    // Info for piecewise defined stock
    std::vector<float> piece_bias   = {0.0, 0.0, 0.0};
    std::vector<float> piece_vol    = {25.0, 25.0, 25.0};
    std::vector<float> piece_times  = {10.0, 20.0};


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
    c_opt_ln_piece.longCall(strike, expiration);

    g_opt_ln.longCall(strike, expiration);
    g_opt_ln_div.longCall(strike, expiration);
    g_opt_ln_disc_div.longCall(strike, expiration);
    g_opt_ln_piece.longCall(strike, expiration);

    std::cout << "CALL VALUE (LN) CPU: " << c_opt_ln.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN) GPU: " << g_opt_ln.getPrice(accuracy) << std::endl;

    std::cout << "CALL VALUE (LN DIV) CPU: " << c_opt_ln_div.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DIV) GPU: " << g_opt_ln_div.getPrice(accuracy) << std::endl;

    std::cout << "CALL VALUE (LN DISC DIV) CPU: " << c_opt_ln_disc_div.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN DISC DIV) GPU: " << g_opt_ln_disc_div.getPrice(accuracy) << std::endl;

    std::cout << "CALL VALUE (LN PIECEWISE) CPU: " << c_opt_ln_piece.getPrice(accuracy) << std::endl;
    std::cout << "CALL VALUE (LN PIECEWISE) GPU: " << g_opt_ln_piece.getPrice(accuracy) << std::endl;




    std::cout << "RNG SPECTRAL PROPERTIES TEST" << std::endl;
    std::vector<size_t> explist = {2, 4, 5, 6, 7};
    size_t Mbase = 10;
    float tgtmean = 100.0;
    float tgtstddev = 35.0;
    float tgtu = 100.0;
    std::cout << "target mean   " << tgtmean << std::endl;
    std::cout << "target stddev " << tgtstddev << std::endl;

    for(size_t& e : explist) {
      size_t M = std::pow(Mbase, e);
      std::vector<float> c_vec(M);
      std::vector<float> g_vec(M);
      auto c_spect = LN<Device::cpu>( tgtmean, 0.0, tgtstddev );
      auto g_spect = LN<Device::gpu>( tgtmean, 0.0, tgtstddev );
      c_spect.getPrices( M, c_vec.data(), tgtu );
      g_spect.getPrices( M, g_vec.data(), tgtu );

      auto c_spect_mean = mean(M, c_vec.data());
      auto c_spect_stddev = stddev(M, c_vec.data(), c_spect_mean);
      c_spect_stddev *= sqrt(TRADING_DAYS_PER_ANNUM / tgtu);

      auto g_spect_mean = mean(M, g_vec.data());
      auto g_spect_stddev = stddev(M, g_vec.data(), g_spect_mean);
      g_spect_stddev *= sqrt(TRADING_DAYS_PER_ANNUM / tgtu);

      std::cout << "M = " << M << std::endl;
      std::cout << ":::CPU:::" << std::endl;
      std::cout << "mean    " << c_spect_mean << std::endl;
      std::cout << "stddev  " << c_spect_stddev << std::endl;
      std::cout << ":::GPU:::" << std::endl;
      std::cout << "mean    " << g_spect_mean << std::endl;
      std::cout << "stddev  " << g_spect_stddev << std::endl;
      std::cout << "=========" << std::endl;
    }

    return 0;
    
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
