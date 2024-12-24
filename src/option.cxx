#include <iostream>

#include <stats.h>
#include <option.h>

namespace OP {
  double CallOption::getPriceKern(size_t N, double* prices) {
    for( size_t it = 0; it < N; it++ ) {
      const double tmp = prices[it];
      if( tmp > strike )
        prices[it] = tmp - strike;
    }
  }


  std::pair<double, double> CallOption::getPrice(float accuracy) {
    double sampleStdDev = accuracy * 2.0;
    double sampleMean   = -1.0;
    
    const size_t N = 100000;
    size_t runMult = 0;
    std::vector<double> optionPrices(N);
    size_t maxiter = 10000;

    while( sampleStdDev > accuracy ) {
      // Get stock price action
      underlying->getPrices(N, optionPrices.data()+runMult*N, expiration);
      // Get option value from that (destructively)
      getPriceKern(N, optionPrices.data() + runMult*N);
      // Get total sample mean
      sampleMean =   mean(optionPrices.size(), optionPrices.data());
      
      sampleStdDev = stddev(optionPrices.size(), optionPrices.data(), sampleMean) / sqrt(optionPrices.size());

      runMult++;
      optionPrices.resize((runMult+1) * N);

      std::cout << "iter " << runMult << std::endl;
      std::cout << "mean " << sampleMean << std::endl;
      std::cout << "std  " << sampleStdDev << std::endl;
      if( runMult > maxiter ) {
        std::cerr << "hit maxiter" << std::endl;
        throw;
      }
    }
    return std::make_pair(sampleMean, sampleStdDev);
  }

}
