FROM segment/chamber:2 AS chamber

FROM bitnami/minideb:buster

LABEL org.label-schema.name="flowmill/k8s-relay" \
      org.label-schema.description="Relays Kubernetes Metadata from k8s-collector to Flowmill" \
      org.label-schema.schema-version="1.0"

# ca-certificates are required by libcurl
RUN apt-get update && apt-get install -y ca-certificates
ENV SSL_CERT_DIR=/etc/ssl/certs

# legacy stuff, we should get rid of references to `/etc/flowtune` at some point
RUN ln -s /etc/flowtune /etc/flowmill

ENTRYPOINT [ "/srv/entrypoint.sh" ]

COPY --from=chamber /chamber /bin/chamber
COPY srv /srv
WORKDIR /srv
RUN if [ ! -e /srv/k8s-relay ]; then \
      ln /srv/k8s-relay-stripped /srv/k8s-relay; \
    fi
