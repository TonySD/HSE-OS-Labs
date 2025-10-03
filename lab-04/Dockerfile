FROM debian:12-slim@sha256:7e490910eea2861b9664577a96b54ce68ea3e02ce7f51d89cb0103a6f9c386e0

RUN apt-get update && apt-get install -y gcc make && rm -rf /var/lib/apt/lists/*

COPY . .

CMD ["make", "test"]