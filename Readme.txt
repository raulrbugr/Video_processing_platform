SDSoC project for 2016.2 version

It is based on this project from Digilent:

https://reference.digilentinc.com/reference/software/sdsoc/reference-manual

The new platform have new functionalities:

- Plugin architecture for new filters
- JPEG encoder to save the processed frames
- 3 new filters*

*If you want to develop a new filter, only have to read the example and add it to addons directory

This platform have a good performance with 38,80 FPS executing the video processing at the reconfigurable hardware.

In the project, the clock in the FPGA and in the hardware function must be in 116.67MHz.

Enjoy!