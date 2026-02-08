# kruskal-maze-generator-c

An infinite, procedurally generated maze written in C using **Kruskal's Algorithm**.  
Each room is generated once, stored persistently, and reused when revisited — creating a seamless infinite maze world.

## Features
- Infinite maze generation using Kruskal’s algorithm
- Procedurally generated rooms with persistent state
- Seamless room-to-room transitions (up, down, left, right)
- World-coordinate–based map storage (no regeneration on revisit)
- Terminal-based rendering with ANSI escape codes
- Written entirely in C

## Algorithm
This project uses **Kruskal’s Algorithm** to generate perfect mazes (no cycles, fully connected).
Each maze room is generated independently and linked together through a global world coordinate system.

## Controls
- `W` – Move up  
- `A` – Move left  
- `S` – Move down  
- `D` – Move right
