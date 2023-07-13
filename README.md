# Fractal-Tiled
This repository contains the implementation of message queuing in "fractal". The client and server are created through make files and run through the exec file created. The majority of the project, which I completed for a class, was written in C++. The method "fractal" is used to produce fractal tiles to an output Bitmap utilizing instances of "FractalMessage" and std::async. A nested for-loop in Main.cpp produces 16 instances of FractaLMessage, one for each tile, and passes these messages to std::async to do the fractal calculation and display. Then, std::async renders each tile in an asynchronous manner.

## Build and Run
In order to build and run this project, use the provided Makefile.
```
make
./executable_name # replace this with the actual name of your generated executable
```

Please ensure that you have the required dependencies installed before running the project.

## Contribution
We welcome any and all contributions to this project! If you're interested in improving our code, adding new features, or just fixing some bugs, please feel free to make a pull request.

## Sources
DISCLAIMER: Most of the code is from and for my course "Advanced Programming". DO NOT COPY FOR CHEATING PURPOSES!!!
