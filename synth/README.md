# Synth
This is a simple virtual mono legato synthesizer I wrote with the JUCE framework for the final assignment of my CS 416P class at Portland State University. Some of the code was initially automatically generated as a template from Projucer, which is JUCE's project wizard application  that I compiled from source. The mono voice is a sawtooth wave with no band limiting/antialiasing, but the envelope has a modifiable attack and release time (5 ms to 4 sec), and the synth itself is velocity-sensitive (note-on only). I didn't find this project to be a huge challenge but it was a little overwhelming getting things going. I may turn this into a personal project outside of school but as far as I know, I already went beyond the minimum requirements.

## Building (Linux)
- [Download JUCE](https://juce.com/download) and build Projucer if a binary of it was not included in the download (this can be done by using the Makefile provided in `<JUCE directory>/extras/Projucer/Builds/LinuxMakefile`).
- Download this directory from this repository.
- Launch Projucer and open the `synth.jucer` file in this directory
- Click `Exporters` on the left-side menu, right click `Linux Makefile`, and click `Save this exporter`. This will generate the directories `JuceLibraryCode` and `Builds`.
- Open a terminal window in `Builds/LinuxMakefile` and enter `make Standalone`.
- Wait for the application to finish building, and it will be called `synth` and located in the `build` directory.

## Libraries
- [JUCE](https://juce.com) under the GNU Affero General Public License version 3 (AGPL v3)

## References
- Another JUCE project of mine that is not yet public (as of Jun 9 2026)
- [Official JUCE documentation](https://docs.juce.com)
- [CPPReference](https://en.cppreference.com)
- possibly one or more other tutorial/help sites

## License
Copyright © 2026 me (Tristan)

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program. If not, see [https://www.gnu.org/licenses/](https://www.gnu.org/licenses/).
