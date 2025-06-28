FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Update & install all required packages in one go
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libeigen3-dev \
    libsfml-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxrender-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /VesicleDiffusion

# Copy the project files (source, headers, CMakeLists.txt, etc.)
COPY . .

# Create build dir and build the project
RUN mkdir -p build
WORKDIR /VesicleDiffusion/build
RUN cmake ..
RUN make

# Run the compiled binary
CMD ["./VesicleDiffusion"]

