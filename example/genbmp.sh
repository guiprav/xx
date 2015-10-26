#!/bin/sh
set -e

# Bitmap header:
xx --char BM # BMP signature
xx --uint32 74 # File size
xx --uint16 0 0 # Reserved fields (must be zero)
xx --uint32 54 # Absolute offset to start of image data

# BITMAPINFOHEADER header:
xx --uint32 40 # Header size (must be 40)
xx --int32 2 2 # Image size (width x height)
xx --uint16 1 # Number of planes in the image (must be 1)
xx --uint16 24 # Bits per pixel
xx --uint32 0 # Compression type (0 = none).
xx --uint32 16 # Size of image data in bytes
xx --uint32 0 0 # Image resolution in PPM
xx --uint32 0 0 # Number of colors and important colors in image (or zero)

# Image data (pixels):
xx --uint8 0xFF 0x00 0x00
xx --uint8 0x00 0xFF 0x00
xx --uint8 0x00 0x00 # Padding
xx --uint8 0x00 0x00 0xFF
xx --uint8 0xFF 0x00 0xFF
xx --uint8 0x00 0x00 # Padding
