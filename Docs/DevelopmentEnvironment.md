\# Development Environment



This document describes the verified development environment for the SMC-MIDI2DMX project.



\---



\# Repository



GitHub:

https://github.com/bbakala/SMC-MIDI2DMX



Main branch:

main



Development branch:

develop



\---



\# Directory Structure



Recommended workspace:



C:\\Users\\bbaka\\git\\



Repository:



C:\\Users\\bbaka\\git\\SMC-MIDI2DMX



Arduino Sketchbook:



C:\\Users\\bbaka\\Documents\\Arduino



\---



\# Git



Verified version:



Git 2.55.0.windows.2



Credential manager:



Git Credential Manager (device authentication)



Recommended workflow:



git pull

...

git add .

git commit

git push



\---



\# Arduino IDE



Verified IDE:



Arduino IDE 2.3.8

Arduino IDE 2.4.x (to be validated)



Sketchbook:



C:\\Users\\bbaka\\Documents\\Arduino



Projects are opened directly from the Git repository.



\---



\# ESP32 Board Package



Current development:



ESP32 by Espressif



Version:

(to be completed)



\---



\# Libraries



Managed using Arduino Library Manager.



Do not store installed libraries in Git.



\---



\# Project Structure



Core/



Common platform-independent code.



Contains:



\- Config

\- FixtureTypes

\- MapSelector

\- DmxEngine

\- SceneManager

\- MidiRaw



Platform folders:



SMC-MIDI2DMX\_ESP32\_WROOM



SMC-MIDI2DMX\_ESP32\_S3



\---



\# Git Releases



Stable releases are created from Git tags.



Example:



v04\_03-rc1



\---



\# Development Rules



\- Develop on the `develop` branch.

\- Keep `main` stable.

\- One Issue = one feature or refactoring.

\- Compile after every small change.

\- Commit after every completed step.

\- Push regularly.

\- Create Release Candidate before closing a milestone.



\---



Last updated



2026-07



\# Architecture



Core contains platform-independent logic.



Platform-specific code should remain only for:



\- BLE

\- USB

\- UART

\- GPIO

\- LEDs

\- Hardware initialization



Business logic belongs in Core whenever possible.

