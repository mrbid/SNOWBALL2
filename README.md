# SNOWBALL2
The GLFW version of Snowball.mobi with MSAA. A novel speed run game akin to a platform loop/buzz wire game.

### How to play

You have to roll around collecting snow and avoiding trees until your snowball is large enough to continue to the next level.

Once you have accumulated a large enough snowball, the poles _(obelisks)_ will turn into teleportation pads to the next round; the aim is to complete each level or a sequence of levels with the highest score in the shortest time.

The larger the snowballs you collect higher the score. Gold snow is double the reward of white. **Picking up snow increases your speed temporarily; the idea is to enter a flow of collecting snow so that your speed stays up.**

Try not to hit the trees as they will slow you down and reduce the mass of your snowball, unless your snowball is large enough to consume them, then they slow you down but also add to the mass of your snowball. Trees will shrink slightly as you approach them to show that they can be consumed.

Your score is updated in the program title bar at the end of each level.

---

### Mouse Controls
```
Mouse 1 (Left)  = Toggle Control
Mouse 2 (Right) = Change View
Mouse X1        = Decrease mouse speed
Mouse X2        = Increase mouse speed
```

---

### Keyboard Mappings
```
ESCAPE        = Close Game
Any other key = Toggle Menu
```

---

### Video
https://youtu.be/oOmu1vtLsik *(old video, no msaa)*

---

### Snapcraft / Software Center
https://snapcraft.io/snowball

---

### App Image
https://github.com/mrbid/SNOWBALL2/raw/main/snowball-x86_64.AppImage

---

### Prerequisites
`sudo apt install libglfw3 libglfw3-dev upx-ucl`

### Linux Binary (Ubuntu 21.10)
https://github.com/mrbid/SNOWBALL2/raw/main/snowball2

---

### Compile & Install
```
gcc main.c -Ofast -lSDL2 -lGLESv2 -lEGL -lm -o snowball
sudo cp snowball /usr/bin/snowball
```

---

### Make & Install
```
make
sudo make install /usr/bin
```

---

This is probably going to be a slow progress project as it's not a priority or a passion project of mine at the moment, essentially I wanted to try out [GLFW](https://www.glfw.org/) and decided that porting [Snowball.mobi](https://github.com/mrbid/Snowball.mobi) to GLFW would be a good test case.

Generally I really like GLFW, although it has no default audio support like SDL has, GLFW feels like a better portable framework to be using for OpenGL. Also, audio support in SDL2 probably wont be as good as utilising [PortAudio](http://www.portaudio.com/).

At the moment this is a direct port of the original Snowball game, but the plan would be to radically improve upon the SDL2 GLES version.

GLFW 16x MSAA | SDL2 GLES NO MSAA
:-------------------------:|:-------------------------:
![snowball screen capture 16x msaa](screenshot.png) | ![snowball screen capture no msaa](https://dashboard.snapcraft.io/site_media/appmedia/2021/09/Screenshot_2021-09-26_11-15-10.png)

The original Snowball.mobi never had MSAA because it was designed to be a OpenGL ES / WebGL game and MSAA is not technically supported, or a great idea to be using on low end devices. The idea of porting Snowball to GLFW is to take advantage of all the features OpenGL has to offer.

Launching as `./snowball2 1337 0 1 1024 1024 0.3 1 1.0 1.5` is pretty cool. ([video](https://youtu.be/n424i-7_4Zw))

## ADDED
- MSAA
- Joystick Support
- Dual Joystick Support
- Window/Borderless mode

## Joysticks
By default the game will boot in double joystick mode, although you can enable single joystick mode via the command line.

The mappings are designed for an Xbox 360 gamepad.

When using joystick mode, the input/mouse sensitivity becomes a master input sensitvity. You can also fine tune the sensitivity of both Joystick 1 & 2. I personally like to have Joy 1 as a slower fine control and Joy 2 as a double speed reflex control. If you have both set to the same sensitivity when they are both engaged you will get double the speed.

`JoyPad Button1`: reduce master sensitvity<br>
`JoyPad Button2`: increase master sensitvity<br>
`JoyPad Button3`: change view<br>
`JoyPad Button10 (LT)`: change view<br>
`JoyPad Button11 (RT)`: change view

When a joystick is connected to the computer the mouse is disabled, the joystick takes priority, but hotswapping is allowed, so just unplug from the USB port to get the mouse back.

**Possible additions:**
- Inverting individual sticks rather than both at the same time.
- Inverting on an axial basis, os only horizontal or vertical invert.

Honestly I don't even know how anyone could play this with inverted sticks, let alone one stick inverted and one not, or one axis inverted etc. Hence not adding this feature, seems like unnecessary complexity for the UI. Although I could add this functionality as command line arguments or via a configuration script in the future if desired.

## TODO
- Audio system
- Particle effects

The shader program... Well this time I am going to make one shader program which executes all four of the shading modes that I make use of. Why? Well if this article is anything to go by _(which I hope it is)_ [GPU inner workings](https://vksegfault.github.io/posts/gentle-intro-gpu-inner-workings/) then technically these if statement switches would be inter-subgroup branching and not intra-subgroup branching which means that when all threads execute they will all be executing the same branches/pathways. In the past my belief was that no branching in shaders should be performed if possible, and I am sure there is still truth to this statement but Snowball is not Crysis and inter-subgroup branching is probably going to be just fine. Also I would imagine inter-subgroup branching would be more performant than state changes for the shader program.

For anyone confused by that terminology ([ref](https://www.merriam-webster.com/words-at-play/intra-and-inter-usage)):<br>
`The prefix intra- means "within" (as in happening within a single thing), while the prefix inter- means "between" (as in happening between two things).`

---

### Lore

I wanted to create something that had that Windows 3.1 [Ski Free](https://en.wikipedia.org/wiki/SkiFree) simplicity, but without the scary monster and the pooping dogs. I had always liked simple puzzle games such as [Neverball](https://neverball.org/) and wanted to go in this kind of direction, and really in a way Snowball is the product of these two games combined. It's the simplicity and style of Ski Free in 3D with the sphere centric concept of Neverball. Snowball itself is designed to be a competitive speed run game, or just something to play when you want to kill time. Snowball produces console output which can be used in competitive games where players compete against one another in a given period of time.

---

### Attributions
https://www.thingiverse.com/thing:946172<br>
http://www.forrestwalter.com/icons/<br>
https://www.flaticon.com/free-icon/computer-mouse_1385<br>
https://www.shutterstock.com/image-illustration/tree-growth-diagram-green-leaf-nature-1299130129<br>
