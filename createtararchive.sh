#! /bin/bash
# script to create a release tarball for strigi
# this is needed since the project is split up in separate git moduls
TAG=0.8.0
git archive HEAD --format=tar --prefix=strigi-$TAG/ | xz > strigi-$TAG.tar.xz

