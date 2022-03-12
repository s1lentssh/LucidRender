cmake_minimum_required(VERSION 2.8)
project(MacOS)

set(SOURCES
   Interface.h
   Interface.m
)

add_library(${PROJECT_NAME} ${SOURCES})
add_library(Lucid::MacOS ALIAS ${PROJECT_NAME})
