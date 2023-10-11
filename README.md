# Space Boy

[![License: MIT](https://img.shields.io/badge/License-MIT-purple.svg)](https://opensource.org/licenses/MIT)

## Description

An intel 8080 emulator written in C.

## Motivations

I've set out to build this project to better understand low-level systems programming, considering my interest in older technologies and games it seemed like a good fit. Intel 8080 emulators seem to be a popular entry point for programmers who are interested in the field, meaning that there are ample resources to help me understand and solve the problem at hand.

The end goal is a fully functional intel 8080 emulator, I'll be testing the functionality with the classic game "Space Invaders".

## Where is the project at now?

Currently the project functions only as a disassmbler for the 8080's instructions. You can clone the repo, navigate to the src directory and compile the `disasm.c` into an executable and run it with a file containing 8080 instructions like so `./{executable file} {8080 instruction file}`

## License

This project is licensed under the MIT license.
