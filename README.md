### Hardware
This can be done on Windows, but it is advisable to use a Raspberry Pi with RPi OS on.

Also see my Hackster & Electromaker pages for the full Pico + MPU6050 h/w build [here](https://www.hackster.io/314reactor/ml-powered-overheating-nerf-barrel-ea5bf8)
and [here](https://www.electromaker.io/project/view/ml-powered-overheating-nerf-barrel/).

### Setting up Pico SDK
Follow the instructions for setting up pre-requisites and the Pico SDK [here](https://circuitdigest.com/microcontroller-projects/how-to-program-raspberry-pi-pico-using-c)

### Setting up the project with Edge Impulse inferencing repository
1. Clone the repository :
```git clone https://github.com/edgeimpulse/example-standalone-inferencing-pico```
2. Clone this repository:
```git clone https://github.com/LordofBone/NerfSuppressor```
3. Drag the contents of the NerfSuppressor folder into the example-standalone-inferencing-pico folder overwriting anything in there.
4. Then go to my Edge Impulse project: [here](https://studio.edgeimpulse.com/public/172613/latest)
5. Go to the "Deployment" tab and download the C++ library (also enabling the EON compiler optimization).
6. Download the int8 model (you can also download the float32 model, but it will be slower).
7. Then drag the 3 folders in the downloaded zip into the 'source' folder under the 'example_inferencing_pico' folder.

### Compiling and flashing
To compile the code, navigate into the build folder and run the following commands:

```cmake ..```

```make```

(can optionally add -j4 to the end of the second command to speed up compilation - changing the number to the number of cores you have)

There should now be a SensorReader.uf2 file in the build folder.

Plug in a Raspberry Pi Pico to your computer while holding the boot button, then let go, the Pico should mount a folder
simply copy the SensorReader.uf2 file to the Pico folder and wait for it to reboot.

Once running you can install and use screen to check it is working and reporting the sensor data.

```sudo apt install screen```

```screen /dev/ttyACM0```