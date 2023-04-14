# **The Smart Compressor**

## **Introduction:**

The Smart Compressor is an audio compressor that changes the volume of a given audio file to be more even and centered around a specified value. Existing audio compressors are difficult to understand and use, so our program is intended to be more intuitive and easy to control than existing options. 

## **Technical Architecture:**

Our application was written primarily in C++ and is comprised of three main components:
* **File Conversion:** This component converts between WAV files and C++ data structures with numeric audio data. It recieves the audio input file from the GUI, converts it to a C++ vector of numeric audio data, and sends it to the audio compression component. It also receives the output data from the audio compression and converts it back to an audio file to send back to the GUI.
* **Audio Compression:** This component executes an algorithm that compresses numeric audio data. It recieves the numeric audio data from the file conversion component, compresses the audio via a compression algorithm, then sends it back to the file conversion component.
* **GUI Interface** This component executes the main interface of the application using several C++ and JUCE libraries. It reads in the audio input file from the user and sends it to the file conversion component. It also recieves the output audio file from the file conversionc component to display to the user.

JUCE is a C++ framework that allows for the incorporation of many musical features. Our GUI was constructed using JUCE libraries. Once the file conversion and audio compression functions were written and tested, they were integrated into our JUCE program for the front-end code to reference. The sequence of functions occurred in the following order:
1. GUI reads in WAV file from user and sends to file conversion.
2. File conversion converts WAV file to numeric audio data held in C++ data structures and sends to audio compression.
3. Audio compression applies the compression algorithm to the data and outputs the compressed audio data.
4. File conversion reads in the compressed audio data, converts to WAV file, and sends back to GUI.
5. GUI displays output file to user.

## **Instructions:**

Download JUCE for your computer from this link: https://juce.com/get-juce/download.

Download the zip file of the project from Github. Once it downloads, extract the zip file. In the folder, navigate to the "SmartCompressorProj" folder. Open the .jucer file in this folder. A Projucer window should pop up. Select your exporter (Visual Studio Code for Windows or Xcode for Mac) and launch. If you chose Visual Studio Code, you must have CMake installed. See the instructions at the bottom of this page. The project will then open in the exporter you chose. To run the application, simply press the play button. The application will build and a pop-up window should appear displaying the application. Building the application may take some time if it is the first time launching the application.

In this application, you can upload audio files, perform compressions based on chosen parameters, visualize the difference in waveforms, and export compressed files to your local directory. Here is a rundown of the different options and features present:

**Open button:** Opens your local file directory to upload a specific WAV file. The waveform for the uploaded file will be displayed in the top section.

**Play button:** Plays the compressed file. This can only be done once a file is successfully compressed. If an audio file is playing, this button will allow you to pause the audio and resume.

**Stop button:** Stops the audio and resets it back to the start of the file.

**Volume slider:** Controls the volume when an audio file is being played.

**Parameters:** Allows the user to enter doubles (ex: 0.7) as parameters for compression. Simply click on the box to enter a number.
* **Maximum Volume:** The desired maximum amplitude of any part of the sound wave, between 0 (silence) and 1 (loudest possible), inclusive (although 0 would not produce a meaningful result).
* **Minimum Volume:** The desired minimum amplitude of any part of the sound wave, between 0 and 1, smaller than maximum volume.
* **Average Volume:** The desired target amplitude to change data points toward, between the maximum and minimum volume.
* **Maximum Adjustment:** A hard limit on how different a data point can be from the original, between 0 and 1. A large value will result in most of the audio flattening to a constant amplitude, and a small value will result in not much change. 
* **Compression Speed:** The number of samples the algorithm takes into account when changing each data point. It is most intuitive to convert this value from seconds into samples using the WAV rate of 44100 samples per second; a value equivalent to a small fraction of a second is recommended.
* **Silence Threshold:** The maximum amplitude where the program considers the segment silence and does not change it. Set this to whatever value between 0 and 1 that suits the silence or ambient noise in your file.

**Compress button:** Compresses the uploaded file. A file must be uploaded using the 'Open button' and all six parameters boxes must be filled for successful compression. Once compressed, the waveform for the compressed audio will be displayed in the bottom section.

**Export to file button:** Exports the compressed file to your local directory, specifically the location of the original file. A file must have been successfully compressed to export (the pathway to the potential exported file should be displayed in the green box).

**Timer:** Displays the current timestamp in seconds of the song being played.

**Waveforms:** Displays the waveforms of the uploaded audio (top) and the compressed audio (bottom). A file must be uploaded for the top waveform to show and a file must be compressed for the botton waveform to show.

The typical sequence of steps should be as follows: 
1. Upload file using the 'Open button'. The uploaded audio waveform will appear in the top section.
2. Enter parameters.
3. Compress file. The compressed audio waveform will appear in the bottom section.
4. Play/pause/stop audio file, set volume, and/or export compressed file.
5. Return to any of steps 1-4.

## **CMake Information:**

Cloning--
In order to include JUCE library with this project, make sure to 
```git submodule init
   git submodule update
```
to install JUCE as a submodule of this project
Building--
To build this project, create a 'build' directory in your current directly and build by:
``` mkdir build ```
``` cd build ```
``` cmake .. ```
``` cmake --build . ```

Executables
To build and run the GUI, make sure to have a configured CMakeListFile under "\PlayingSoundFilesTutorial\Builds\LinuxMakefile",
build and a GUI executable will be generated

To install cmake in a global directory:
```git clone https://github.com/catchorg/Catch2.git```
```cd Catch2```
```cmake -Bbuild -H. -DBUILD_TESTING=OFF```
```sudo cmake --build build/ --target install ```
