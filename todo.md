# TODO
* Fix bug in physics leading to really jittery bouncing objects: every frame, the object is pushed into the ground by gravity and then bounces back up; I suspect this really needs forces to be applied continuously over the duration of the frame but this requires some really fucky math. Basically to get perfect accuracy I'd have to predict where a parabolic arc intersects a line which is really terrifying for a lot of reasons -- e.g. a lot of culling techniques don't work because in theory the object could do a 180 in one frame and collide with an object behind it. I think the best way to deal with it is probably to:
    * When speed is applied, add it to a "new speed" pool.
    * Advance the object in the direction of its total speed, including new speed.
    * Move speed from the new speed to the old speed pool proportional to the distance traveled.
    * Reflect the object but do not reflect the new speed pool.
    * Repeat the process.
    This won't give pixel-perfect accuracy, but it should allow acceleration applied directly into a surface (e.g. gravity) to be properly dissipated.
* Better testing of physics edge cases (pun intended)
* Debug visualization
* Keyboard input
* Short barriers
* Barrier integration into grids etc
* Sound integration?
* Multiple game planes
* Peek-through for lower planes
* 2.5D compatible tileset?
* runtime tile type loading???
* Test level
* Enemy logic/alerting system