# FISH GAME!

Group members: Chris Butulis (@chrisbutulis), Roberto Gonzalez (@RobertoGonzalesMatos), Dan Hu (@danhu0), 
Eitan Zemel (@EitanZe)

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
and loaded in from .glb files. It is spaced out inside of the map using a Poisson disc sampling clustering
algorithm.

**Particle generation**: The particles spawn in using a particle generation algorithm originating at a
single point in space. There are implementations for fireworks and geysers, and the fireworks go off
when the fisherman catches the fish.

**Animation**: There are swimming animations which work by rotating and translating the nodes of the 
glTF models of the fish and fisherman between certain bounds.

## Credits
Occasional use of ChatGPT.

Fisherman model adapted from [WIP - Swimming dude](https://sketchfab.com/3d-models/wip-swimming-dude-5562c2217f8e467785c654f97d232879)
made by Loïc Norgeot and uploaded to Sketchfab.

Fish model adapted from [Rainbow Trout - Redband](https://sketchfab.com/3d-models/rainbow-trout-redband-442ab10476d0486e8eb689941bc4ad6e)
made by eb78 and uploaded to Sketchfab.

Coral models sourced from the Smithsonian

All project handouts can be found [here](https://cs1230.graphics/projects).
