FROM debian:latest

WORKDIR /install

RUN apt update
RUN apt install -y curl git clang make cmake
RUN apt install -y netcat-openbsd valgrind neovim
RUN apt install -y nginx

RUN curl -sS https://starship.rs/install.sh -o starshipinstall.sh
RUN sh starshipinstall.sh --yes
RUN echo 'eval "$(starship init bash)"' >> ~/.bashrc

# Install Catch2
RUN git clone https://github.com/catchorg/Catch2.git
RUN cd Catch2 && cmake -B build -S . -DBUILD_TESTING=OFF && cmake --build build/ --target install
