# Pull base image.
ARG python_version
FROM python:${python_version}-slim-buster

# must be redefined after FROM
ARG PYTHON_VERSION=$PYTHON_VERSION 
ARG BAZEL_VERSION=3.2.0
ARG BAZEL_INSTALLER=bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh
ARG BAZEL_DOWNLOAD_URL=https://github.com/bazelbuild/bazel/releases/download

# Set environment variables
ENV HOME=/root
ENV PROJECT_DIR="${HOME}/PyDP"
ENV PATH="/root/bin:${PATH}"
ENV DP_SHA="b7f4c39d9f73d67b34cdbd1b8483e5f72072fc73"

# Define working directory
WORKDIR ${HOME}

# Install apt-get packages
RUN apt-get update && \
    apt-get -y install \
    sudo \
    wget \
    zip \
    git \
    software-properties-common \
    gcc \
    g++ \
    build-essential \
    python3-distutils \
    pkg-config \
    zlib1g-dev

# get third-party dependencies
WORKDIR /tmp/third_party
# TODO: peg this dependency to particular version. Can pass args in for latest or stable etc
RUN git clone https://github.com/google/differential-privacy.git 
RUN pip3 install pipenv

# Change working dir
WORKDIR ${PROJECT_DIR}

# Copy local source over
COPY . ${PROJECT_DIR}

# Dealing with 3rd party code one needs to pin it to a version or a SHA.
# Either we clone it here as was done but then it should be @e819e03 to match the submodule 
# and will needed to be updated in keeping with that (a rather tedious way)
# Get google dp dependency. Rather copy the thrid party deps as it's a cloned submodule
# COPY ./third_party third_party

# RUN mkdir -p third_party && \
#     cd third_party && \
#     git clone https://github.com/google/differential-privacy.git

# Or we just copy it in and ensure the CI/CD clone the repo and the submodule.
# Then all keeps in sync.
# Remove unused java code
RUN rm -rf third_party/differential-privacy/java && \ 
    rm -rf third_party/differential-privacy/examples/java

# This makes the pipenv's virtual environment in the project dir 
ENV PIPENV_VENV_IN_PROJECT=true 

# Build the bindings using Bazel and create a python wheel
RUN pipenv --python ${PYTHON_VERSION} && \
    pipenv run bazel build src/python:bindings_test  --verbose_failures

RUN cp -f ./bazel-bin/src/bindings/pydp.so ./pydp && \
    rm -rf dist/ && \
    pipenv run python setup.py bdist_wheel && \
    pipenv install dist/*.whl

# This `activates` the virtual env
ENV VIRTUAL_ENV=$PROJECT_DIR/.venv
ENV PATH="$VIRTUAL_ENV/bin:$PATH"
# Default entrypoint
CMD ["/bin/bash"]
