#include <iostream>

#include <stats.h>
#include <option.h>

namespace OP {
  double CallOption::getPriceKern(size_t N, double* prices) {
    double value = 0.0;
    for( size_t it = 0; it < N; it++ ) {
      const double tmp = prices[it];
      if( tmp > strike )
        value += tmp - strike;
    }
    value /= N;
    return value;
  }

  // Calculates option price
  // Gets stock prices until the last M mean option prices have a maximum diff of less than float accuracy
  double CallOption::getPrice(float accuracy) {
    double runningMeanDiff = 100.0;
    double runningMean   = -1.0;
    
    const size_t N = 1000000;
    const size_t M = 100;
    size_t MCounter = 0;
    size_t counter = 0;
    std::vector<double> optionPrices(N);
    bool ready = false;

    while( not ready ) {
      // Get stock price action
      underlying->getPrices(N, optionPrices.data(), expiration);
      // Get option value from that (destructively)
      auto sampleMean = getPriceKern(N, optionPrices.data());
      // Get total running mean and difference from last
      runningMeanDiff = runningMean;
      runningMean = ( (double)counter * N * runningMean + N * sampleMean ) / ((double)(counter + 1) * N);
      runningMeanDiff = std::abs( runningMean - runningMeanDiff );
      
      counter++;

      // Exit if last M running mean diffs are below threshold
      if( runningMeanDiff < accuracy )
        MCounter++;
      else
        MCounter = 0;

      if( MCounter == M )
        ready = true;
      
      std::cout << runningMean << " " << runningMeanDiff << std::endl;
        
    }
    return runningMean; 
  }

}
