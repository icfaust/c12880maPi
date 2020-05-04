
#include <vector>
#include <cmath>

template <typename T> T gaussian(std::vector<float> x, std::vector<T> spectrum, float alpha, float mu, float sigma1, float sigma2) {
  float output, squareRoot;
  for(int i=0; i < x.size(); i++){
      squareRoot = (x[i] - mu)/(x[i] < mu ? sigma1 : sigma2);
      output += (float)spectrum[i]*alpha*std::exp( -(squareRoot * squareRoot)/2);
  }
  return alpha * exp( -(squareRoot * squareRoot)/2 );
}

template <typename T> std::vector<T> xyzFromWavelength(std::vector<float> wavelength, std::vector<T> spectrum) {
  std::vector<T> xyz = {0,0,0};
  
  xyz[0] = gaussian(wavelength, spectrum,  1.056, 599.8, 3.79, 3.10)
         + gaussian(wavelength, spectrum,  0.362, 442.0, 1.60, 2.67)
         + gaussian(wavelength, spectrum, -0.065, 501.1, 2.04, 2.62);

  xyz[1] = gaussian(wavelength, spectrum,  0.821, 568.8, 4.69, 4.05)
         + gaussian(wavelength, spectrum, 0.286, 530.9, 1.63, 3.11);

  xyz[2] = gaussian(wavelength, spectrum, 1.217, 437.0, 1.18, 3.60)
         + gaussian(wavelength, spectrum, 0.681, 459.0, 2.60, 1.38);

  return xyz;
}

template <typename T> std::vector<T> xyz2rgb(std::vector<float> xyz){
  std::vector<T> rgb = {0,0,0};

  rgb[0] = 3.24096994*xyz[0] - 1.53738318*xyz[1] - .49861076*xyz[2];
  rgb[1] = -.96924364*xyz[0] + 1.8759675*xyz[1] + .04155506*xyz[2];
  rgb[2] = .055630098*xyz[0] - .20397696*xyz[1] + 1.05697151*xyz[2];

  // I need to do a gamma correction here somehow

  return rgb;

}