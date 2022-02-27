cmake_minimum_required(VERSION 3.12.4)

# Project Setup
project(stb VERSION 0.0.0.1 LANGUAGES CXX)

add_library(stb STATIC)
target_sources(stb PRIVATE "stb_image.cpp")
