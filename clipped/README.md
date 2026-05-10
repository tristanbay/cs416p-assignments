# Clipped (CS 416P Assignment)
This is my program for the "Clipped" assignment in the CS 416P course (Computers, Sound, and Music) at Portland State University.

## Setup
Install the `libsndfile` and `portaudio` development packages for your Linux distro. The name and command will vary depending on your package maintainer.
- For Fedora: `sudo dnf install libsndfile-devel portaudio-devel`
- For Ubuntu and Debian: `sudo apt install libsndfile-dev portaudio-dev`

## Compilation
Simply run `make` on the command line in the project directory.

## Execution
Run the command `./clipped <sine or clipped>` in the project directory.
- The parameter is the mode choice (sine if no parameter given):
    - `sine` makes the program output a WAV file named `sine.wav` which contains a 1-second sine wave tone at 440 hz and at 1/4 of maximum amplitude.
    - `clipped` makes the program output a WAV file named `clipped.wav` which contains the same as above except twice the previous amplitude but hard-clipped to still be the same 1/4 of maximum amplitude.
Both options play the generated audio.

## Troubleshooting
If the audio stutters/is intermittent, try changing the `BUFSIZE` macro on line 17 to a larger value.

## Libraries Used
libsndfile ([under the LGPL version 2.1 or 3](https://www.gnu.org/licenses/lgpl-3.0.html))
portaudio ([under the MIT Expat license](https://files.portaudio.com/docs/v19-doxydocs/License.html))

## References Used
[this source code/web page](https://files.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html)

## Licensing
Copyright © 2026 me (Tristan)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see [https://www.gnu.org/licenses/](https://www.gnu.org/licenses/).
