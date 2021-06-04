FROM    ubuntu:latest

# Install necessary components
ENV     DEBIAN_FRONTEND=noninteractive 
RUN     apt-get update; \
        apt-get install -y build-essential; \
        apt-get install -y libgl-dev libglu1-mesa-dev; \
        apt-get install -y libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev;

# Add source and build it
COPY    ./ src/
WORKDIR src
RUN     make install

CMD     ./BilliardsGame