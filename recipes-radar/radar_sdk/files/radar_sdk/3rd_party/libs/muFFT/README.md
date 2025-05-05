# muFFT

muFFT is a library for doing the fast fourier transform (FFT) in one or two dimensions.
The FFT has many applications in digital signal processing.
The main use cases are fast linear convolution and conversion from time domain into frequency domain and vice versa.
See [The Fast Fourier Transform](@ref FFT) for details on how the algorithm works and how it is implemented in muFFT.

## Features

muFFT is a moderately featured single-precision FFT library.
It focuses particularly on linear convolution for audio applications and being optimized for modern architectures.

 - Power-of-two transforms
 - 1D/2D complex-to-complex transform
 - 1D/2D real-to-complex transform
 - 1D/2D complex-to-real transform
 - 1D fast convolution for applying large filters.
   Supports both complex/real convolutions and real/real convolutions.
   The complex/real convolution is particularly useful for filtering interleaved stereo audio.
 - Designed and optimized for SIMD architectures,
   with optimizations for SSE, SSE3 and AVX-256 currently implemented.
   ARMv7 and ARMv8 NEON optimizations are expected to be implemented soon.
 - Radix-2, radix-4 and radix-8 butterfly implementations.
 - Input and output does not have to be reordered, as is sometimes the case with FFT algorithms.
   muFFT implements the Stockham autosort algorithm to avoid any explicit permutation of FFT coefficients.
 - Detects SIMD support for your hardware in runtime.
   Same muFFT binary can support wide ranges of hardware feature sets.

## Building

muFFT is built with straight CMake. Use `add_subdirectory` in your project.

muFFT uses the C99 and C++ ABI for complex numbers, interleaved real and imaginary samples, i.e.:

```
struct complex_float {
	float real;
	float imag;
};
```

C99 `complex float` from `<complex.h>` and C++ `std::complex<float>` from `<complex>` can safely be used with muFFT.

## Performance 

muFFT is written for performance and is usually competitive with highly optimized libraries like FFTW3 and FFmpeg/libavcodec FFT.
See [Benchmark](#benchmark) for how to run your own benchmarks.

muFFT is designed with moderate size FFTs in mind.
Very large FFTs which don't fit in cache could be better optimized by designing for cache utilization
and tiny FFTs (N = 2, 4, 8) don't have special handcoded vectorized transforms.

muFFT does not need to run micro benchmarks ahead of time to determine optimal FFT decompositions,
as is supported in more sophisticated FFT libraries. Reasonable decompositions are found statically.

## License

The muFFT library is licensed under the permissive MIT license, see COPYING and preambles in source files for more detail.

## Documentation

The public muFFT API is documented with doxygen.
Run `doxygen` to generate documentation. Doxygen 1.8.3 is required.

After running Doxygen, documents are found in `docs/index.html`.
