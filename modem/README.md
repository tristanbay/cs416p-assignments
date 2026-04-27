# Modem (CS 416P Assignment)
This is my program for the "Modem" assignment in Portland State University's CS 416P (Computers, Sound, and Music) course.

## Setup
Install the `libsndfile` development package for your Linux distro. The name and command will vary depending on your package maintainer.
- For Fedora: `sudo dnf install libsndfile-devel`
- For Ubuntu and Debian: `sudo apt install libsndfile-dev`

## Compilation
Simply run `make` on the command line in the project directory.

## Execution
In the project directory, add a WAV file called `message.wav` that has a sample rate of 48 kHz, is 16-bit resolution, and is mono (1 audio channel). It should be a message where text is encoded as ASCII, but each byte of data is sandwiched in between two chunks of 160 samples representing starting and ending bits, and the bits in the byte of data itself are 160 samples of either a 2025 hz tone representing a `0` or a 2225 hz tone representing a `1`. This format was adapted from the answering portion of the Bell 103 modem protocol.  
  
Run the command `./modem` in the project directory to display the encoded message. It can be redirected to a filename to (re)create that file or append more data to an existing file.

## Licensing
Copyright © 2026 me (Tristan)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [https://www.gnu.org/licenses/](https://www.gnu.org/licenses/).
