#include <iostream>

#include <stats.h>
#include <option.h>

namespace OP {
  float CallOption::getPriceKern(size_t N, const float* prices) {
    float value = 0.0;
    for( size_t it = 0; it < N; it++ ) {
      const float tmp = prices[it];
      if( tmp > strike )
        value += tmp - strike;
    }
    value /= N;
    return value;
  }

  float PutOption::getPriceKern(size_t N, const float* prices) {
    float value = 0.0;
    for( size_t it = 0; it < N; it++ ) {
      const float tmp = prices[it];
      if( tmp < strike )
        value += strike - tmp;
    }
    value /= N;
    return value;
  }

  // Calculates option prices in position
  // Gets stock prices until the last M mean option prices have a maximum diff of less than float accuracy
  float OptionPosition::getPrice(float accuracy) {
    float expiration = 100;
    if( not options.empty() )
      expiration = options[0]->expiration;

    float runningMeanDiff = 100.0;
    float runningMean   = -1.0;
    
    const size_t N = 100000;
    const size_t M = 20;
    size_t MCounter = 0;
    size_t counter = 0;
    std::vector<float> stockPrices(N);
    bool ready = false;

    while( not ready ) {
      // Get stock price action
      underlying->getPrices(N, stockPrices.data(), expiration);
      // Get option value from that
      float sampleMean = 0.0;
      for( std::shared_ptr<Option> o : options )
        sampleMean += o->getPriceKern(N, stockPrices.data());
      
      // Get stock price value
      sampleMean += mean(N, stockPrices.data()) * (float)stockPosition;

      // Get total running mean and difference from last
      runningMeanDiff = runningMean;
      runningMean = ( (float)counter * N * runningMean + N * sampleMean ) / ((float)(counter + 1) * N);
      runningMeanDiff = std::abs( (runningMean - runningMeanDiff) / runningMeanDiff );
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

