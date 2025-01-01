#include <iostream>
#include <map>
#include <set>

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
    if( isShort ) return -1.0*value;
    else return value;
  }

  float PutOption::getPriceKern(size_t N, const float* prices) {
    float value = 0.0;
    for( size_t it = 0; it < N; it++ ) {
      const float tmp = prices[it];
      if( tmp < strike )
        value += strike - tmp;
    }
    value /= N;
    if( isShort ) return -1.0*value;
    else return value;
  }

  // Calculates option prices to desired accuracy (percent) and at desired time (days)
  // If time is negative, calculates option price at last expiry
  // If stock-only position and no time is given, throws
  
  // Gets stock prices until the last M mean option prices have a maximum diff of less than float accuracy
  float OptionPosition::getPrice(float accuracy, float time) {
    // Check if position has any stock/options in it
    if( options.empty() and stockPosition == 0 )
      return 0.0;
    if( options.empty() and time < 0.0 ) {
      std::cerr << "ERROR: Time not specified in OptionPosition.getPrice with stock-only position" << std::endl;
      throw;
    }

    // Keeps track of how long out to calculate price action for each option
    // A nullptr value will represent the stock position
    std::multimap<float, std::shared_ptr<Option>> TimesAndOptions;
    std::set<float> expirations; // for convenience
    
    if( time < 0.0 ) {
      for(std::shared_ptr<Option> o : options) {
        expirations.insert(o->expiration);
        TimesAndOptions.insert({o->expiration, o});
      }
      // If theres a stock position, calculate it out to the last expiration
      if( stockPosition != 0 ) {
        auto maxTime = std::max_element(expirations.begin(), expirations.end());
        TimesAndOptions.insert( {*maxTime, nullptr} );
        expirations.insert(time);
      }
    }
    else { // custom time is specified
      for(std::shared_ptr<Option> o : options) {
        const auto thisOptionsExpiry = std::min(time, o->expiration);
        expirations.insert(thisOptionsExpiry);
        TimesAndOptions.insert({thisOptionsExpiry, o});
      }
      if( stockPosition != 0 )
        TimesAndOptions.insert( {time, nullptr} );
        expirations.insert(time);
    }

    

    // Setup convergence parameters
    float runningMeanDiff = 100.0;
    float runningMean   = -1.0;
    accuracy *= 0.10;
    
    const size_t N = 100000;      // Fetch new prices in blocks of this size
    const size_t M = 10;          // Stop when the last M prices have changed by less than desired accuracy
    size_t MCounter = 0;
    size_t NCounter = 0;
    std::vector<float> stockPrices(N);
    bool ready = false;
    size_t maxiter = 1000;

    while( not ready ) {
      float sampleMean = 0.0;
      for( const float& time : expirations ) {
        // Get stock price action at time
        underlying->getPrices(N, stockPrices.data(), time);
        // Get list of options at time
        auto optionsAtTime = TimesAndOptions.equal_range(time);
        // Iterate over them
        for( auto to = optionsAtTime.first; to != optionsAtTime.second; ++to) {
          auto o = to->second; // should be shared_ptr<Option>
          if( o != nullptr )  // If not stock position, get option price
            sampleMean += o->getPriceKern(N, stockPrices.data());
          else                // If stock position, get stock price
            sampleMean += mean(N, stockPrices.data()) * (float)stockPosition;
        }
      }

      // Get total running mean and difference from last
      runningMeanDiff = runningMean;
      runningMean = ( (float)NCounter * N * runningMean + N * sampleMean ) / ((float)(NCounter + 1) * N);
      runningMeanDiff = std::abs( (runningMean - runningMeanDiff));
      NCounter++;
      
      // Exit if we've done too many iterations
      if( NCounter > maxiter ) {
        std::cerr << "OptionPosition.getPrice has exceeded max iterations" << std::endl;
        return runningMean;
      }


      // Exit if last M running mean diffs are below threshold
      if( runningMeanDiff < accuracy )
        MCounter++;
      else
        MCounter = 0;

      if( MCounter == M )
        ready = true;
      
    }
    return runningMean; 
  }
}

