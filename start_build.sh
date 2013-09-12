#!/bin/bash

gyp --depth . build.gyp

make leveldb_ex -j5 
