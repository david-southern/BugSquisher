# BugSquisher

This 'portfolio repo' is intended to give an idea of some of the projects I have worked
on in the past. 

BugSquisher was an iOS game I built based upon some of the early-childhood reading and 
phonics ideas that I learned while working on [ELF](https://github.com/david-southern/ELF).

I was looking to learn iOS developemnt, and I thought it would be fun to build a kid-friendly
phonics game.  The initial idea was to have bugs crawl across the screen with various phonemes
labeled on thier back.  The child would then squish only those bugs that `did|did not` create
a valid word with the current level's phoneme prefix.  After developing the first level, I added three
three more levels with the same basic phoneme management, but with different gameplay manipulation, 
requiring the children to pick apples from a tree, drive dumptrucks through a sandbox to build a 
sandcastle, and place bricks in a garden wall.  

I used the Marmalade game engine, although at the time is was called AirPlay, which is 
why that name shows up in the code.  This was my first return to C++ after around 8 years away, so
it might be a bit rough here and there.  The game made it to a beta state where I did some basic
testing with several of my nieces and nephews, to generally positive reviews, but I dropped it
at that point, as "real world" work picked up and my interest turned to Unity development.

One thing that I particularly like about this effort was the 
[GameFramework](https://github.com/david-southern/BugSquisher/tree/master/source/GameFramework)
interface.  I built this to manage the game sprites and transformations in a general manner.  This
made the creation of individual level/playing piece activity much easier.  It used
a heirarchical sprite/transformation model so that you could "layer" transforms and sprites together
to create more complex behavior.  For example, you could have a bug that was a PathFollowingSprite 
that folowed a complex Bezier curve around the screen.  Anchored to that you could have an FlipBook
sprite to animate the bug's legs as it moved, and a static Sprite with an offset Anchor that used
and OrientTo transformation to have the bug look in a particular direction.

The GameFramework also included a basic state model engine and a future event scheduler which made
building the game logic much easier.  This allowed the level code to focus on the level-specific
actions.