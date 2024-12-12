# FISH GAME!

Group members: Chris Butulis (@chrisbutulis), Roberto Gonzalez (@RobertoGonzalesMatos), Dan Hu (@danhu0), 
Eitan Zemel (@EitanZe)

## How to run
1. Open both the FishGameClient project and FishGameServer project in Qt
2. Navigate to FishGameServer
3. Run the following sequence of commands (note: you may need to install cmake if it's not 
already installed, and if you're on MacOS, you'll need a package manager like homebrew)

``$ rm build`` ``$ mkdir build`` ``$ cd build `` ``$ cmake .. `` ``$ cd ..``

4. Then set the desired port in main.cpp
5. Now you can run ./run_server.sh, which will build and start the server
7. Switch the project to FishGameClient
8. Change your build directory to FishGameClient/build/CORRESPONDING_BUILD (i.e. MinSizeRelease). This
is important so relative paths work correctly for model imports. 
9. Set the ip and port in FishGameClient/src/realtime.cpp to the ip and port for the server (use ifconfig
to find internal ip for the server on brown subnet)
10. Run the client twice on the same computer or once on two separate computers
11. Have fun!  

## Overview
FISH GAME is a first-person underwater multiplayer fish game. One player plays as Prof. Daniel Ritchie
(the fisherman), and the other plays as a fish. The game is played on a locally hosted server using a TCP 
connection on a shared network. There is a 30 second timer; the object of the game for the fisherman is
to catch the fish, and the fish is trying to run out the timer and survive.

The controls are <kbd>W</kbd><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd> and <kbd>Space</kbd> for movement, and 
<kbd>Left Mouse Button</kbd> for looking around. The fisherman has a fishing rod which can be cast by holding down
and releasing <kbd>Left Mouse Button</kbd>. If the end of the line collides with the fish, the fish changes
color and is caught.

## Main Graphical Components
This project is built off of Projects 5 & 6 of CSCI1230 for the OpenGL pipeline implementation. The main
new graphical features are the fishing rod, terrain generation, particle generation, and animation.

**Fishing rod**: The fishing rod works using a line composed of primitive cylinders. The physics are
determined using inverse kinematics. Collision detection uses a radius from the center of the opponent
and checks to see if the end of the line enters into the radius.

**Terrain generation**: The terrain is composed of different coral models sourced from the Smithsonian
and loaded in from .glb files. It is spaced out inside of the map using a Poisson disc sampling algorithm
modified for generating coral clusters. Models are loaded using the tinygltf libary.

**Particle generation**: The particles spawn in using a particle generation algorithm originating at a
single point in space. There are implementations for fireworks and geysers, and the fireworks go off
when the fisherman catches the fish.

**Animation**: There are swimming animations which work by rotating and translating the nodes of the 
glTF models of the fish and fisherman between certain bounds.

**Networks**: The networks component was developed for the networks final project, and was inspired 
by the snowcast project earlier in the class. The server runs on a port on localhost, accepting 
clients, and starts a timer when both the fish and daniel ritchie client are connected (these 
are assigned based on who joins the game first). The client side runs two threads: one to handle 
server updates, and one to send periodic updates in realtime.cpp. src/utils/networksclient.cpp 
is the API on the client-side for sending and receiving. In the update handler, ctm updates for the 
opponent are received as well as timer countdown updates.

## Credits
Occasional use of ChatGPT.

Tinygltf library for loading models (located in lib directory)

Coral generation was adapted Robert Bridson's algorithm for [Fast Poisson Disk Sampling in Arbitrary Dimensions](https://www.cs.ubc.ca/~rbridson/docs/bridson-siggraph07-poissondisk.pdf)

Fisherman model adapted from [WIP - Swimming dude](https://sketchfab.com/3d-models/wip-swimming-dude-5562c2217f8e467785c654f97d232879)
made by Loïc Norgeot and uploaded to Sketchfab.

Fish model adapted from [Rainbow Trout - Redband](https://sketchfab.com/3d-models/rainbow-trout-redband-442ab10476d0486e8eb689941bc4ad6e)
made by eb78 and uploaded to Sketchfab.

Coral models sourced from the Smithsonian on Sketchfab: [Brain Coral](https://sketchfab.com/3d-models/pseudodiploria-strigosa-8db353e299634cf1bfeca6f6d112582d),
[Tall Coral](https://sketchfab.com/3d-models/stylaster-sanguineus-4f1ddd8352944d16bf3b821b3e71b473), [Generic Coral](https://sketchfab.com/3d-models/pocillopora-damicornis-caespitosa-662c5b9144334e678b871f1960c03392)