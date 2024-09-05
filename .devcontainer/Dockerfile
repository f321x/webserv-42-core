FROM debian:latest

WORKDIR /install

RUN apt-get update && apt-get install -y curl git clang make

RUN curl -sS https://starship.rs/install.sh -o starshipinstall.sh
RUN sh starshipinstall.sh --yes
RUN echo 'eval "$(starship init bash)"' >> ~/.bashrc
