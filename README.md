# Escape from the Funky Factory

> A chapter of featherless' [digital creative history](https://github.com/featherless/digital-creative-history).

In the summer of 2003 I attended the National Youth Leadership Forum on Technology. In the months leading up
to the event me and a group of other attendees created a group called The Developers Alliance. We made a game.

The name of the game was Escape from the Funky Factory. It was a puzzle game.

![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/preview.gif)

![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/intro.png)
![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/credits.png)
![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/credits2.png)

## Levels

![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/level1.png)
![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/level2.png)
![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/level3.png)
![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/level20.png)

## Editors

I made a level editor and data viewer application for use by the team to create levels.

![Screenshot](https://raw.githubusercontent.com/featherless/2003-EscapeFromTheFunkyFactory/master/gfx/viewer.png)

## Version history

    CURRENT VERSION: v1.0

    :::V1.0 to final:::
    -Fixed the loading of music so that if the song is already loaded,
      it won't load it again
    -Fixed dialog boxes for passwords and high scores
    -Random other bugs

    :::v0.93 to v1.0:::
    -Fixed lasers so they draw BEHING Timmy, not over him.
    -Made it so the timers don't count if you're paused.
    -Fixed some bugs in the high score system.
    -Made it so if you die, the bgmusic doesn't have to reload itself every time.
    -Got rid of the enable/disable for the goal obs, it's kinda pointless being in there :)
    -Fixed a minor bug in the level editor that was causing the timers and multi-obs to not
      be drawn....It seemed to have something to do with the "turn affected obstacle red"
      thing.....who knows ;)
    -Added laser tutorial finally.  Seems I forgot to put it in initially.....
    -Made lasers be drawn on top-most layer so that they can be placed over other obstacles now.
    -Added conveyor belt and button sound effects.  They are kind of hard to hear :( because if
      the background music is loud, it drowns them out.

    :::v0.92 to v0.93:::
    -Fixed playlist buffer size: increased it to 64 characters in case you have long song names.
    -Added feature to level editor:
     -When you hold control while placing obstacles, it will act as sort of a "paste", by copying
      the last obstacle placed exactly and making a duplicate of it.
    -Fixed the cranes at the end of each level so that they reset.
    -Added Timers and high scores.  Each level automatically is generated its own high scores file.
     -Highest score is displayed at the bottom of the level while you are playing it.
     -Best scores are automatically logged in and saved to the file.  Keeps track of each person's
      name.

    :::v0.91 to v0.92:::
    -Added playlist feature so that all songs in the "Music" folder have a chance of being played.
    -Made it so when you step on a button, all of the obstacles that it affected turn red.
    -Finished fixing all of the tutorial levels with the new hammers.
    -Made the button thing ONLY change colors of obstacles if a BUTTON toggled it, timers aren't
      counted.
    -Timmy doesn't stop mid-animation when he steps on a crane, he goes to the "standing" position.

    :::v0.9 to v0.91:::
    -Fixed bug with level increments, it would show the passwords in "Load Custom" mode
    -Fixed song loading in between levels in "play custom" mode
    -Made LoadSong function reset to the default directory after loading the song
    -Fixed AVI Player, set the filtering to linear and made it so it didn't resize the image,
      should look a bit nicer now.
    -Fixed font set, yay!

    :::v0.85 to v0.9:::
    -Added 2 features to the level editor:
     -Made it so the current obstacle you are editing has a semi-transparent white triangle over
      it.
     -Made it so when you delete an obstacle in the list, the other ones keep their settings and
      don't get offset.
    -Added SynchAll to the SWITCH trigger

    :::v0.8 to v0.85:::
    -Added the password system, there are 20 passwords, so it can automatically work for the next
      few levels.
    -Suggesting to add some better passwords.......the ones I put in are just completely random,
      we need to think of a bunch of eight letter words :)
    -Made the passwords get displayed at the beginning of each level, during the "transition"
      stage, now all we need are pics, ROAR!!!!
    -Made the transition screen stay longer (eight seconds, to be exact), or until you hit space/
      escape/enter to skip out of it.
    -Fixed a bug with the crane where, in Disco mode, it wouldn't change color.......
    -Fixed hammer coldet, it was offset on the up/down hammers, on the left side by about 8 pixels
    -Screw the above ^^, I had the hammers completely offset to be drawn to 48 pixels instead of
      64 :(.......this means that they were aligned to 1.5 tiles instead of 2, that is fixed
      now however....I'm sorry I didn't notice this earlier, I had to do so much yard work
      the past couple of days, rafgagrfhgf

    :::v0.73 to v0.8:::
    -New hammers are finally in!! yaaay
     -Collision detection is working great for them
    -Made it so that if you hit escape in the "you're dead" screen, the menu pops up
    -Spectrum analyzer won't crash if the song isn't playing and you go in to the menu
    -Songs repeat now!!
    -If you die on any level, you can go right back in to it again from the "you're dead" screen
      this saves a lot of hassle

    :::v0.72 to v0.73:::
    -Made it so you can quit the menu screen by hitting escape, it was annoying having to always
      hit space to get out :)
    -FINALLY fixed the stupid avi intro thing after about 2 hours of staring at the code trying
      to figure out what was missing, turns out I had forgotten to set the texture parameters
      for the filters.
    -MUST FIX THE WORD-WRAPPING FOR THE REST OF THE TUTORIALS!!!!
    -Fixed up font.bmp a bit so that it's easier on the eyes.
    -Need to add thing for obstacle copying/pasting and remembering the state of the last obstacle

    :::v0.71 to v0.72:::
    -Got rid of options screen, I didn't like it
    -Temporary cheat codes are just set by hitting keys.
     -The only cheat so far is Disco mode, which is set by hitting "D".
     -Cheats only work INGAME
    -Made Tutorial mode, complete with 8 levels that introduce you to the gameplay and all the
      obstacles.
    -Fixed mouse bug in the level editor so you only have to click once now, and not have to
      click twice all the time.
    -Fixed lasers so their images don't reset
    -Added in-game menu
    -Added Spectrum Analyzer to the in-game menu.
    -Got rid of options screen totally, I didn't like it :)
    -Made it so the animations do NOT move on the conveyor belts, so Timmy doesn't moon walk
      while on the conveyor belt.
    -Fixed ALL the obstacle's toggle code, so we don't have to worry about it anymore.
    -Note: There IS a debugger in the level editor, just hit enter.
    -Added buttons that you can press in the menu of the level editor.
    -Debug mode shows the ID value of the object you're editing.
    -Holding shift while in debug mode will show the tile pos, not the pixel pos.
    -You can reset the bmp pictures and also the "Next Level", mid-edit in the level editor.
    -Saved a bit in the multi-obs's code (no pun intended).
    -Fixed Timmy's stopped animation so that when he stops, he stops on the right animation
      and doesn't stop midstride.
    -Made it so you can't make Timmy turbo-walk by holding up/down/left/right at the same time.
    -Fixed the arrows in the level editor so you can't click two at the same time.
    -You can right click now on the timers so that you don't have to zoom past the values.
     -Right clicking makes the value increment only by one value.
    -Added an "Offset" value to the timers, making it possible to offset when things turn on
      and off.

    :::v0.61 to v0.71:::
    -Added random sound track order between the three songs we have so far....
    -Timed the TMA sound just right....I think
    -Added AVI Code
    -fixed slight bug in conveyor pics
    -changed goal pic (changed settings in obstacle.cpp to accomodate)
    -made prog more dependent on SCREENX and SCREENY, instead of hardcoded 800x600
    -fixed pressing-shift-doubles-speed bug (more or less)
    -official levels are now complete up to level 08 (fully tilemapped, linked together, etc)
    -new tilemaps included
    -tweaked collision code for smoother changes of direction

    :::v0.5 to v0.61:::
    -Cleaned up code
    -Was whipped by Dav *sob*

    :::v0.4 to v0.5:::
    -Sped up the obstacles so they don't move like slugs
    -Put frame limiting in the level editor
    -Added bg music (Finally!!)
    -Centered the conveyor belt
    -Added -win thing to level editor
    -Added dying screen
    -Fixed misc bugs I found

    :::v0.3b to v0.4:::::
    -Added Credits, may be missing some people however....
    -Fixed timers and found a way so that I won't need to modify the
      save structure now.
    -Added title screen *yay*
    -Added one crane animation so that the crane goes up before moving
      over to the destination.
    -Fixed crane speed for computers running 110-170 fps.
    -Added SetFace command so I could modify the timer without having
      to remake the save structure.
    -Added Timer obstacle
     -Allows you to select an object to toggle
     -Also allows you to set the delay, so you can have something toggle
     -Every few milliseconds, or every few seconds, whichever you choose
    -Not perfect yet, the only way to make it completely perfect is by
      modifying the save structure, so I might have to do that in
      a bit.
    -Fixed one bug with the multi-obs where if you deleted the object
      that it was pointing at and then went back to change the
      multi-obs, it would crash.  Now it doesn't.
    -Fixed above bug that was similar for all obstacles that pointed at
      something (button, timer, multi-obs)
    -Fixed a LITTLE TINY BUG that was causing a BIG problem :)
      It was with the multi-obs, I didn't put a parenthesis in the
      equation for finding the value of the third obstacle, so
      sometimes, the equation would not work correctly.


    :::v0.3a to v0.3b:::::
    -Added command so that if you hold control when placing a tile,
      it sets the tile to the last set value (from either
      Fill Grid, or Fill Area)
    -Added fps-indy code to the turntables so if you're like me, and
      get 200+fps, you don't spin in to the ground at mach 5.
    -Fixed cranes so they only go a few tiles above the destination,
      making it look a bit more realistic.
    -Finally updated the version number in the save files :)


    ::::v0.3 to v0.3a:::::
    -Fixed bugs with cranes:
     -Now when the crane drops you off, you can keep moving.
    -Fixed character animations, so that you don't glide everywhere
    -Fixed FPS-indy code, again :)
    -Modified the Object map so Dav and mine pix are gone
    -Made it so crane runs appx same speed on all computers
     -If your puter gets low fps (70 and below), the crane moves faster
      to make up for the speed
     -If your puter is faster (above 70), then the crane moves slower,
      but smoother because it technically is going faster....if
      you get that at all :)
    -Changed bg color on the Level Editor to black
     -As a result, had to give the mouse a white outline.
    -Added Area Fill command:
     -Hotkey: T
     -Allows you to select, and fill a certain area of the screen
     -Speeds up fill times, immensely :) (I was getting PO'ed at having
      to click hundreds of times to fill in a little box with tiles)


    ::::v0.2c to v0.3:::::
    -Added cranes
    -Dav fixed fps-indy code
    -Added features to the level editor including:
     -Hold shift to lock the position of objects to tiles
     -Cranes are very easy to edit and modify
    -Fixed bugs in DisDir code (so only one direction can be
      disabled at a time)


    ::::v0.2b to v0.2c::::
    -Added lasers
    -Made it so multi-obs can have either 2 or 3 obstacles being pointed to
    -Fixed some bugs in drawing and deleting code
    -Fixed misc. bugs all over the place.

# License

All source code is licensed Apache 2.0.

> A chapter of featherless' [digital creative history](https://github.com/featherless/digital-creative-history).
