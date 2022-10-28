# IC
Information and Codification Course

# How to compile
```
cd ./sndfile-example-src
make
```

# How to run 
```
cd ./sndfile-example-bin
```

## Exercise 2 - wav_hist
Outputs data for creating a histogram.

Usage:
```
./wav_hist <input_file> <channel>
```
Examples:
```
./wav_hist ../examples/sample.wav MID
./wav_hist ../examples/sample.wav SIDE
./wav_hist ../examples/sample.wav 0
./wav_hist ../examples/sample.wav 1
```

## Exercise 3 - wav_quant
Reduce the number of bits used to represent each audio sample (uniform scalar quantization).

Usage:
```
./wav_quant <input file> <output file> <num_bits> <version>
```
Examples:
```
./wav_quant ../examples/sample.wav output.wav 4 1
./wav_quant ../examples/sample.wav output.wav 4 2
./wav_quant ../examples/sample.wav output.wav 8 1
./wav_quant ../examples/sample.wav output.wav 8 2
```

## Exercise 4 - wav_cmp
Outputs the Signal-to-noise ratio (SNR) of the input file in relation to the original file, as well as the maximum per sample absolute error.

Usage:
```
./wav_cmp <input file> <output file>
```
Examples:
```
./wav_cmp ../examples/sample.wav output.wav
```

## Exercise 5 - wav_effects
Produces audio effects

Usage:
```
./wav_effects <input file> <output file> <delay> <amplitude_eco> <mode>
```
Examples:
```
./wav_effects ../examples/sample.wav output.wav 2 0.5 SINGLE
./wav_effects ../examples/sample.wav output.wav 2 0.5 MULTIPLE
./wav_effects ../examples/sample.wav output.wav 2 0.5 AMPLITUDE_MOD
./wav_effects ../examples/sample.wav output.wav 2 0.5 TIME_VARYING
```

## Exercise 7 - BitStream
Read and write bits to a file

Usage:
```
./BitStream [-r (def)]
            [-w] 
            <text_file> <bin_file>
```
Examples:
```
./BitStream -r output_binary_data ../examples/data 
./BitStream -w ../examples/data output_binary_data
```

## Exercise 8 - wav_dct_codec and wav_dct_decoder
Encode and decode a file based on the DCT block by block.

### wav_dct_codec
Usage:
```
wav_dct_codec [ -v (verbose) ]
            [ -bs blockSize (def 1024) ]
            [ -frac dctFraction (def 0.2) ]
            outputCodecFile wavFileIn
```
Examples:
```
./wav_dct_codec output_bits_file ../examples/sample.wav
```

### wav_dct_decoder
Usage:
```
./wav_dct_decoder [ -bs blockSize (def 1024) ]
                    inputCodecFile wavFileOut
```
Examples:
```
./wav_dct_codec output_bits_file ../examples/sample.wav
```