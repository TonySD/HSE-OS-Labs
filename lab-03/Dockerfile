FROM debian:12-slim@sha256:7e490910eea2861b9664577a96b54ce68ea3e02ce7f51d89cb0103a6f9c386e0

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y debconf ca-certificates build-essential && \
    echo "postfix postfix/main_mailer_type select Local only" | debconf-set-selections && \
    echo "postfix postfix/mailname string mail.local" | debconf-set-selections && \
    apt-get install -y postfix bsd-mailx \
    && rm -rf /var/lib/apt/lists/*

RUN postconf -e "myhostname = mail.local" \
    && postconf -e "myorigin = \$myhostname" \
    && postconf -e "mydestination = \$myhostname, localhost.localdomain, localhost" \
    && postconf -e "inet_interfaces = loopback-only" \
    && postconf -e "inet_protocols = all" \
    && postconf -e "append_dot_mydomain = no" \
    && postconf -e "mailbox_size_limit = 0" \
    && postconf -e "biff = no"

RUN mkdir -p /var/mail && chgrp mail /var/mail && chmod 2775 /var/mail

WORKDIR /root
COPY . .

CMD ["make", "test"]
