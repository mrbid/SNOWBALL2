# SNOWBALL2
The GLFW version of Snowball.mobi with MSAA.

This is probably going to be a slow progress project as it's not a priority or a passion project of mine at the moment, essentially I wanted to try out [GLFW](https://www.glfw.org/) and decided that porting [Snowball.mobi](https://github.com/mrbid/Snowball.mobi) to GLFW would be a good test case.

Generally I really like GLFW, although it has no default audio support like SDL has, GLFW feels like a better portable framework to be using for OpenGL. Also, audio support in SDL2 probably wont be as good as utilising [PortAudio](http://www.portaudio.com/).

At the moment this is a direct port of the original Snowball game, but the plan would be to radically improve upon the SDL2 GLES version.

GLFW 16x MSAA | SDL2 GLES NO MSAA
:-------------------------:|:-------------------------:
![snowball screen capture 16x msaa](screenshot.png) | ![snowball screen capture no msaa](https://dashboard.snapcraft.io/site_media/appmedia/2021/09/Screenshot_2021-09-26_11-15-10.png)

The original Snowball.mobi never had MSAA because it was designed to be a OpenGL ES / WebGL game and MSAA is not technically supported, or a great idea to be using on low end devices. The idea of porting Snowball to GLFW is to take advantage of all the features OpenGL has to offer.

Launching as `./snowball2 1024 1024 0.3 1337 1` is pretty cool. ([video](https://youtu.be/n424i-7_4Zw))

## ADDED
- MSAA
- Joystick Support
- Dual Joystick Support
- Window/Borderless mode

## Joysticks
By default the game will boot in single joystick mode, although you can enable double joystick mode via the command line.

The mappings are designed for an Xbox 360 gamepad.

When using joystick mode, the input/mouse sensitivity becomes a master input sensitvity. You can also fine tune the sensitivity of both Joystick 1 & 2. I personally like to have Joy 1 as a slower fine control and Joy 2 as a double speed reflex control. If you have both set to the same sensitivity when they are both engaged you will get double the speed.

This is how I personally like to launch my config but it really depends on the controller you are using, MasterSensitvity: 0.3, DoubleJoysticks: 1, Joy1: 1.0, Joy2: 1.5:<br>
`./snowball2 1024 768 0.3 1337 0 1 1.0 1.5`

`JoyPad Button1`: reduce master sensitvity<br>
`JoyPad Button2`: increase master sensitvity<br>
`JoyPad Button3`: change view<br>
`JoyPad Button10 (LT)`: change view<br>
`JoyPad Button11 (RT)`: change view

When a joystick is connected to the computer the mouse is disabled, the joystick takes priority, but hotswapping is allowed, so just unplug from the USB port to get the mouse back.

## TODO
- GL Menu System
- Audo system
- Particle effects
- Deprecate esAux for vec/mat or linmath.

The shader program... Well this time I am going to make one shader program which executes all four of the shading modes that I make use of. Why? Well if this article is anything to go by _(which I hope it is)_ [GPU inner workings](https://vksegfault.github.io/posts/gentle-intro-gpu-inner-workings/) then technically these if statement switches would be inter-subgroup branching and not intra-subgroup branching which means that when all threads execute they will all be executing the same branches/pathways. In the past my belief was that no branching in shaders should be performed if possible, and I am sure there is still truth to this statement but Snowball is not Crysis and inter-subgroup branching is probably going to be just fine. Also I would imagine inter-subgroup branching would be more performant than state changes for the shader program.

For anyone confused by that terminology ([ref](https://www.merriam-webster.com/words-at-play/intra-and-inter-usage)):<br>
`The prefix intra- means "within" (as in happening within a single thing), while the prefix inter- means "between" (as in happening between two things).`

## Prerequisites
`sudo apt install libglfw3 libglfw3-dev`

## Linux Binary
https://github.com/mrbid/SNOWBALL2/raw/main/snowball2
