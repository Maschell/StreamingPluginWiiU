# ScreenStreaming for the Wii U [![Build Status](https://api.travis-ci.org/Maschell/StreamingPluginWiiU.svg?branch=master)](https://travis-ci.org/Maschell/StreamingPluginWiiU)

## Still an early PROOF OF CONCEPT. DON'T EXPECT MAGIC.

This is just a simple plugin that allows you to stream the content of the Gamepad or TV screen to your Computer. With default settings streams in a resolution of 428x240 with selft adjusting quality and tries to achieve as much fps as possible.  
It's possible to adjust the resolution via the config menu (Press **L, DPAD DOWN and MINUS** on your Wii U Gamepad whenever using the home menu is allowed).

But general notes:
- This is still an early PoC.
- Encoding is done in software, not hardware.
- It probably affects gameplay. Loading times are increased, games could lag. I wouldn't recommend you to play online.
- All settings are hardcoded. In the future it will (hopefully) be possible to choose the screen to stream (TV or DRC), at which resolution and which quality.
- No streaming of the home menu.
- Probably unstable.
- Some games might be too dark, some might be too bright, some doesn't work at all.
- Currently streaming is achieved via a custom Java client. 

## Configuration
While the plugin is running, is possible to configure certain parameters. To open the config menu press **L, DPAD DOWN and MINUS** on your Wii U Gamepad. For more information check the [Wii U Plugin System](https://github.com/Maschell/WiiUPluginSystem). 
Currently the following options are available:
- Change the resolution, possible options: 240p, 360p and 480p
- Choose the screen to stream, possible options: Gamepad, TV.


# Usage
Simply load the plugin with the plugin loader, after that start the [StreamingPluginClient](https://github.com/Maschell/StreamingPluginClient). The StreamingPluginClient requires a computer with Java 8. Double click on the `.jar` and enter the IP address of your Wii U console.

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
