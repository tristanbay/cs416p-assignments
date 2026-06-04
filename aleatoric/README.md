# Aleatoric (CS 416P Assignment)
This is my program for the Aleatoric assignment in my Computers, Sound, and Music class at Portland State University. It generates a "song" when it is run based on a handful of chord progressions, a few tunings (equal temperaments), a range of tempos and root pitches, and random chance. The generated notes are all eighth notes and are saw waves with a linear decay that I chose to implement as an extra thing.

## Setup
Install the `libsndfile` and `portaudio` development packages for your Linux distro. The name and command will vary depending on your package manager/package repo maintainer.
- For Fedora: `sudo dnf install libsndfile-devel portaudio-devel`
- For Ubuntu and Debian: `sudo apt install libsndfile-dev portaudio-dev`

## Compilation
Run `make` on the command line in the project directory after you've downloaded the files.

## Execution
Run the command `./aleatoric` in the project directory. This will play the newly generated tune and leave a WAV file of it in the directory.

## Libraries Used
libsndfile ([under the LGPL version 2.1 or 3](https://www.gnu.org/licenses/lgpl-3.0.html))
portaudio ([under the MIT Expat license](https://files.portaudio.com/docs/v19-doxydocs/License.html))

## References Used
the `clipped` assignment also in this repo
maybe [this source code/web page](https://files.portaudio.com/docs/v19-doxydocs/paex__record_8c_source.html) as well
