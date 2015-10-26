#!/bin/sh
set -e

echo -- Bitmap header
echo BMP signature: "$(xx -r --char 2)"
echo File size: "$(xx -r --uint32)"
echo Reserved fields: "$(xx -r --uint16 2)"
echo Absolute offset to start of image data: "$(xx -r --uint32)"

echo
echo -- BITMAPINFOHEADER header
echo Header size: "$(xx -r --uint32)"
echo Image size \(width x height\): "$(xx -r --int32 2)"
echo Number of planes in image: "$(xx -r --uint16)"
echo Bits per pixel: "$(xx -r --uint16)"
echo Compression type: "$(xx -r --uint32)"
image_data_size="$(xx -r --uint32)"
echo Size of image data in bytes: "$image_data_size"
echo Image resolution in PPM: "$(xx -r --uint32 2)"
echo Number of colors in image: "$(xx -r --uint32)"
echo Number of important colors in image: "$(xx -r --uint32)"

echo
echo -- Image data \(pixels\)
xx -r --uint8 "$image_data_size"
