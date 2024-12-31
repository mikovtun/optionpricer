#include "stock.h"


namespace OP {
  template class LN<Device::cpu>;
  template class LN<Device::gpu>;
  template class LNDividend<Device::cpu>;
  template class LNDividend<Device::gpu>;
  template class LNDiscreteDividend<Device::cpu>;
  template class LNDiscreteDividend<Device::gpu>;


  template <Device d>
  void LN<d>::getPrices(size_t N, float* out, float u) {
    LN<d>::getLNModelPrices(N, out, u, start, bias, volatility);
  }

  template <Device d>
  void LNDividend<d>::getPrices(size_t N, float* out, float u) {
    LN<d>::getPrices(N, out, u);
    for(size_t i(0); i < N; i++) {
      out[i] *= std::exp( -1.0 * dividendYield * u );
    }
  }

  template <Device d>
  void LNDiscreteDividend<d>::getPrices(size_t N, float* out, float u) {
    LN<d>::getPrices(N, out, u);
    int numDividendsPaid = std::floor((u - daysTillFirstDividend) / dividendInterval);
    if( numDividendsPaid > 0) {
      for(size_t i(0); i < N; i++) {
        out[i] *= std::pow(1.0 - dividendYield, numDividendsPaid);
      }
    }
  }

  template <Device d>
  void LNPiecewise<d>::getPrices(size_t N, float* out, float u) {
    float curTime = 0;
    std::vector<float> prices(N);
    // Initialize output
    for(size_t n(0); n < N; n++)
      out[n] = start;
    // Update prices piecewise
    for(size_t i(0); i < bias.size(); i++ ) {
      // Two times, current and next, crawling along
      float nextTime;
      if( i == bias.size() ) nextTime = u;
      else                   nextTime = std::min(times[i], u);
      float thisU = nextTime - curTime;
      curTime = nextTime;
      LN<d>::getLNModelPrices(N, prices.data(), thisU, start, bias[i], volatility[i]);
      // Multiply old prices by new ones
      for(size_t n(0); n < N; n++ ) {
        prices[n] /= start;
        out[n] *= prices[n];
      }
    }
  }

  template void LNDividend<Device::cpu>::getPrices(size_t N, float* out, float u);
  template void LNDividend<Device::gpu>::getPrices(size_t N, float* out, float u);

  template void LNDiscreteDividend<Device::cpu>::getPrices(size_t N, float* out, float u);
  template void LNDiscreteDividend<Device::gpu>::getPrices(size_t N, float* out, float u);

  template void LNPiecewise<Device::cpu>::getPrices(size_t N, float* out, float u);
  template void LNPiecewise<Device::gpu>::getPrices(size_t N, float* out, float u);
}
