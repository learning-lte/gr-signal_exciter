#ifndef INCLUDED_SIGNAL_BASE_HPP
#define INCLUDED_SIGNAL_BASE_HPP


#include <vector>
#include <complex>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <signal_exciter/signal_threaded_buffer.h>

//boost stuff
#include <boost/thread/mutex.hpp>
#include <boost/random/random_device.hpp>
#include <boost/math/special_functions/sinc.hpp>

//gr stuff
#include <gnuradio/random.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/filter/firdes.h>

//eigen stuff
#include <eigen3/Eigen/Dense>


typedef std::complex<float> complexf;
typedef std::complex<double> complexd;

#ifndef M_2_PIl
#define M_2_PIl (2*M_PIl)
#endif

class Signal_Base
{
  private:
//    static bool d_seeded;
//    static int d_seed;

    void convolve_2x_1( std::vector<float> &out,
                        std::vector<float> &in2x,
                        std::vector<float> &keep );

    void convolve(std::vector<float> &out,
                  std::vector<float> &a,
                  std::vector<float> &b );

    void norm_f(std::vector<float> &to_norm, float scale=1.);
    void norm_d(std::vector<double> &to_norm, double scale=1.);

    void print_f(std::string Name, float* vec, size_t len);
    void print_fcr(std::string Name, complexf* vec, size_t len);
    void print_d(std::string Name, double* vec, size_t len);

    virtual void auto_fill_symbols() = 0;
    virtual void auto_fill_signal() = 0;

    //For thread safe create of fftw objects.
    static boost::mutex s_mutex_fftw;
    //static boost::mutex s_mutex_fso;
    static boost::mutex* d_mutex_ptr;
    static size_t s_indicator;

  protected:
    boost::random_device d_rd;
    gr::random *d_rng;
    size_t d_indicator;

    bool d_enable_fractional;
    double d_fso;

    virtual boost::mutex& fftw_lock() const {return *d_mutex_ptr;}
    //virtual boost::mutex& fso_lock() const {return s_mutex_fso;}

    void prototype_augemnt_fractional_delay(
          double interp, double interpd, std::vector<float> &proto,
          double frac_delay, std::vector<float> &taps);

    void get_indicator(){
      boost::mutex::scoped_lock scoped(fftw_lock());
      d_indicator = s_indicator++;
    }

  public:
    virtual ~Signal_Base() = 0;

    void enable_fractional_offsets(bool enable, double fso=0.)
    {
      d_enable_fractional = enable;
      d_fso = fso;
    }
    virtual void generate_signal(complexf* output, size_t sample_count) = 0;
    virtual void generate_symbols(complexf* output, size_t symbol_count) = 0;

    static void set_fftw_mutex(boost::mutex* ext_mutex){ d_mutex_ptr = ext_mutex; }
//    int get_seed();
};

inline Signal_Base::~Signal_Base()
{}

#endif /* INCLUDED_SIGNAL_BASE_HPP */
