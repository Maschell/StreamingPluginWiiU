# ScreenStreaming for the Wii U [![Build Status](https://api.travis-ci.org/Maschell/StreamingPluginWiiU.svg?branch=master)](https://travis-ci.org/Maschell/StreamingPluginWiiU)

## Still an early PROOF OF CONCEPT. DON'T EXPECT MAGIC.

This is just a simple plugin that allows you to stream the content of the DRC to any browser.  
Currently no configuration without recompiling is supported. It streams in a resolution of 428x240 and tries to achieve 20 fps. These numbers might improve in the future.

But general notes:
- This is still an early PoC.
- Encoding is done in software, not hardware.
- It probably affects gameplay. Loading times are increased, games could lag. I wouldn't recommend you to play online.
- All settings are hardcoded. In the future it will (hopefully) be possible to choose the screen to stream (TV or DRC), at which resolution and which quality.
- No streaming of the home menu.
- Probably unstable.
- Some games might be too dark, some might be too bright, some doesn't work at all.
- Currently streaming is achieved via "MJPEG via HTTP", this might change in the future to improve performance.

# Usage
Simply load the plugin with the plugin loader. When the system menu is loaded, you can open `http://<ip of your ip>:8080` on your browser and should see the stream. Whenever you switch the application (e.g. load a game), you need to refresh the site in your browser.  
Example when the IP of your Wii U is 192.168.0.44.
```
http:/192.168.0.44:8080
```
If you don't know the IP of your Wii U, you can start for example [ftpiiu](https://github.com/dimok789/ftpiiu) which shows the IP when running.

## Wii U Plugin System
This is a plugin for the [Wii U Plugin System (WUPS)](https://github.com/Maschell/WiiUPluginSystem/). To be able to use this plugin you have to place the resulting `.mod` file into the following folder:

```
sd:/wiiu/plugins
```
When the file is placed on the SDCard you can load it with [plugin loader](https://github.com/Maschell/WiiUPluginSystem/).

## Building

For building you need: 
- [wups](https://github.com/Maschell/WiiUPluginSystem)
- [wut](https://github.com/decaf-emu/wut)
- [libutilswut](https://github.com/Maschell/libutils/tree/wut) (WUT version) for common functions.

Install them (in this order) according to their READMEs. Don't forget the dependencies of the libs itself.

Other external libraries are already located in the `libs` folder.

- libjpeg
- [libturbojpeg](https://libjpeg-turbo.org/)
