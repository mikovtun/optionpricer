namespace OP {

class Option {
  protected:
    Option() = default;
  public:
    Stock* const underlying;
    float strike, expiration;

    virtual ~Option() = default;

    virtual float getPrice(float accuracy) = 0;
};

class CallOption : Option {
  public:
    CallOption() = delete;
    CallOption(Stock* const und, float st, float exp) : underlying(und), strike(st), expiration(exp) { };

    float getPrice(float accuracy) {
      size_t superN = 1000;
      
      return 0.0;
    }


};

}
