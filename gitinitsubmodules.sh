#!/bin/bash

echo "Checking out all submodules..."
git submodule update --init --recursive --progress
echo "Done."
