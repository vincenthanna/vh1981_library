

class Locker
{
  public:
    static Locker &get()
    {
        static Locker __singleton__;
        return __singleton__;
    }

    int value() { return _value; }

  private:
    Locker()
        : _value(0)
    {
    }
    int _value;
};
