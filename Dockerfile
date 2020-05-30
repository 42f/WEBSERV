# Build a wordpress server

FROM debian:10

MAINTAINER bvalette <bvalette@student.42.fr>

RUN apt-get update

EXPOSE 80

CMD ["bash"]
