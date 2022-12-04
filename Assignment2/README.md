# IC
Information and Codification Course - Assignment 2

# How to compile
```
cd ./opencv-src
make
```

# How to run 
```
cd ./opencv-bin
```

## Exercise 1 - cp_image
Copy and transform images.

Usage:
```
./cp_image  [-r degree (def 90)]
            [-i intensity_factor (def 2)]
            <mode> <original_img> <output_img>

```
Examples:
```
./cp_image COPY ../pics-examples/lena.ppm output.ppm
./cp_image NEGATIVE ../pics-examples/lena.ppm output.ppm
./cp_image MIRROR_HORIZONTAL ../pics-examples/lena.ppm output.ppm
./cp_image MIRROR_VERTICAL ../pics-examples/lena.ppm output.ppm
./cp_image -r 90 ROTATE ../pics-examples/lena.ppm output.ppm
./cp_image -r -90 ROTATE ../pics-examples/lena.ppm output.ppm
./cp_image -i 1.5 INTENSITY ../pics-examples/lena.ppm output.ppm
./cp_image -i 0.5 INTENSITY ../pics-examples/lena.ppm output.ppm
```

## Exercise 3 - audio_encoder and audio_decoder
Encode and decode audio file.

### audio_encoder
Usage:
```
./audio_encoder [ -p predictor (def 4) ]
                [ -w window size (def 50) ]
                [ -q num_bits ]
                input_file output_file
```
Examples:
```
./audio_encoder -p 0 -w 100 -q 14 ../audio-examples/sample01.wav compressed.bin
```

### audio_decoder
Usage:
```
./audio_decoder input_file output_file
```
Examples:
```
./audio_decoder compressed.bin output.wav
```

## Exercise 4 - image_encoder and image_decoder
Encode and decode image file.

### image_encoder
Usage:
```
./image_encoder [ -p predictor (def 0) ]
                [ -w window size (def 50) ]
                input_file output_file
```
Examples:
```
./image_encoder -p 0 -w 10 ../pics-examples/airplane.ppm compressed.bin
```

### image_decoder
Usage:
```
./image_decoder input_file output_file
```
Examples:
```
./image_decoder compressed.bin output.ppm
```
