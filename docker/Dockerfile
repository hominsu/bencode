# build stage
ARG ALPINE_VERSION
FROM alpine:${ALPINE_VERSION} AS builder
# intstall git build-base
RUN apk add --no-cache git build-base cmake
# build app
COPY . /src
WORKDIR /src
RUN cmake -H. -Bbuild \
    -DCMAKE_BUILD_TYPE=Releasec \
    -DBENCODE_BUILD_EXAMPLES=ON \
    -DBENCODE_BUILD_TESTS=ON \
    -DBENCODE_ENABLE_INSTRUMENTATION_OPT=OFF \
    -DBENCODE_BUILD_ASAN=OFF \
    -DBENCODE_BUILD_UBSAN=OFF && \
    cmake --build ./build --parallel $(nproc) && \
    GTEST_COLOR=TRUE ctest --test-dir ./build/test --output-on-failure && \
    cmake --install ./build

# final stage
ARG ALPINE_VERSION
FROM alpine:${ALPINE_VERSION}
# image info
ARG AUTHOR_NAME
ARG AUTHOR_EMAIL
ARG VERSION
# label
LABEL author=${AUTHOR_NAME} email=${AUTHOR_EMAIL} alpine_version=${ALPINE_VERSION} version=${VERSION}
# cpoy the app from builder
COPY --from=builder /usr/local/include/bencode /usr/local/include/bencode
COPY --from=builder /usr/local/lib/cmake/bencode /usr/local/lib/cmake/bencode
COPY --from=builder /usr/local/lib/pkgconfig/bencode.pc /usr/local/lib/pkgconfig/bencode.pc
