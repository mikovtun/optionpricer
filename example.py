import optionpricer

#Some handy functions
def LongCallSETable(opts : optionpricer.OptionPosition):
    print("Long Call")
    strikes = [ 50, 90, 100, 110, 150]
    expirydates = [ 7, 14, 21, 45, 100, 365 ]
    print("Strike\t|\t", end="")
    for e in expirydates:
        print(e, end="\t")
    print()
    for s in strikes:
        print(s, end="\t|\t")
        for e in expirydates:
            opts.longCall(s, e)
            price = opts.getPrice(0.01)
            opts.clear()
            print(format(price,".2f"),end="\t")
        print()

def PriceOverTime(opts : optionpricer.OptionPosition, calcTimes):
    print("Time",end="\t")
    for t in calcTimes:
        print(t,end="\t")
    print()
    print("Price",end="\t")
    for t in calcTimes:
        price = opts.getPrice(0.01, t)
        print(format(price,".2f"),end="\t")
    print()


# Define a new ticker MIKE with the following parameters
avgprice = 100.0
bias = 0.0
vol = 50.0              # Percent per year

#Standard lognormal stock model, using the GPU
MIKE = optionpricer.LN_gpu( avgprice, bias, vol)

# Generate an options chain for a long call with given strikes and expiration dates
opts = optionpricer.OptionPosition( MIKE )

print("Constant volatility model")
LongCallSETable(opts)




# Example of volatility crush
# Earnings for MIKE is coming up in 15 days, we expect volatility to go from 50% to 10%
bias_vec = [0.0, 0.0]
vol_vec  = [vol, 10.0]
time_vec = [15.0]

# Set up piecewise Log-normal stock model
MIKE_volcrush = optionpricer.LNPiecewise_gpu( avgprice, bias_vec, vol_vec, time_vec )
opts.setUnderlying(MIKE_volcrush)

print("Volatility crush model (after 15 days)")
LongCallSETable(opts)




opts.clear()
opts.longCall(100.0, 45)
opts.longPut(100.0, 45)
credit = opts.getPrice(0.001)
debit  = opts.getPrice(0.001, 30)
print("Profit from selling 100Straddle45DTE and buying back after 30 days: ", credit - debit )

