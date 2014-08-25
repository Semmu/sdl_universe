SDL_Universe
============

SDL_Universe is a small and basic software renderer written in C++ with SDL2. It is just a hobby project for my own entertainment, but you can use it as you want, and also every contributor is welcome here.



Disclaimer
----------

This is purely a hobby project, I started it to learn and experience 3D programming closely. It should not be compared to modern 3D graphic libraries, such as OpenGL or Direct3D. One could see it as a totally needless and unnecessary project, but I want to work on it, so I will. Feature request and improvement tips are welcome, but I want to keep the project very simple, keep that in mind.



Planned or already implemented features
------------------

* Render colored points, lines and filled triangles positioned in 3D space (these are the primitives)
* SU::Model to bundle these primitives, also to save and load them via a file
* SU::Transformation to transform SU::Models (rotate, scale, mirror, translate, everything)
* SU::Object tree container to group and easily manipulate SU::Models (transform a group of them at once, etc.)
* Fast average distance-based shorting, optional Z-buffer
* Very basic lighting system (ambiance, directional and point light sources)
* "Collision" detectiton, line and triangle cutting appropriately (for example if a line goes through a filled triangle, the line should be cut to 2 pieces)
* **Documentation**
* ...



"Wishlist" features
-------------------

* Texturing
* Multiple backends (immediate OpenGL for example)
* A dynamic Level of Detail system
* Alpha channel for colors (for example half-opaque triangle)
* Higher level bodies (sphere, cylinder, cone, etc.) and runtime tessellation
* ...



What is done
------------
* Can bundle primitives in a SU::Model and render them (well, triangle drawing isn't implemented yet)
* Can create tree-like SU::Object hierarchy
* Transformations work and inherit properly



Notes and known bugs
-----

* The way to display an SDL_Surface with SDL2 is a bit complicated. You need an SDL_Window, an SDL_Renderer for that window, an SDL_Texture to render, and finally an SDL_Surface to create the texture from.
	It seems that the process of this takes up 9ms on my machine, no matter if I draw 1 single line on a surface or a hundred. I should find a faster way to display a single SDL_Surface in the window.

	* Probably SDL_UpdateTexture() is what slows down the process, from 3ms to 10ms latency.

* When something is too close to the projection plane (which is a plane of z=0), the program will freeze when that thing tries to go behind it. Temporary workaround in instanceIsVectorOnScreen(), because it compares to 0.01 not 0.
* Only the triangles facing the eye should be rendered. Don't forget to implement this.
* The current implementation is **not** idiot-proof, use everything as intended. Variable and function names are pretty straightforward, but if something does not work, feel free to ask.


Licence
-------

Not yet chosen, likely zlib or CC-by.



Contact
-------
You can contact me here on GitHub or via email at iamsemmu AT gmail DOT com
