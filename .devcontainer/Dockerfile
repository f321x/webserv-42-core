FROM debian:latest

WORKDIR /install

RUN apt-get update
RUN apt install -y curl git clang make cmake python3 python3-venv python3-requests g++
RUN apt install -y netcat-openbsd valgrind neovim pkg-config libssl-dev
RUN apt install -y nginx

RUN curl -sS https://starship.rs/install.sh -o starshipinstall.sh
RUN sh starshipinstall.sh --yes
RUN echo 'eval "$(starship init bash)"' >> ~/.bashrc

# Install Catch2
RUN git clone https://github.com/catchorg/Catch2.git
RUN cd Catch2 && cmake -B build -S . -DBUILD_TESTING=OFF && cmake --build build/ --target install

# install rust
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
RUN mkdir -p /workspace/rust_target
