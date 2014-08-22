SDL_Universe
============

SDL_Universe is a small and basic software renderer written in C++ with SDL2. It is just a hobby project for my own entertainment, but you can use it as you want, and also every contributor is welcome here.



Disclaimer
----------

This is purely a hobby project, I started it to learn and experience 3D programming closely. It should not be compared to modern 3D graphic libraries, such as OpenGL or Direct3D. One could see it as a totally needless and unnecessary project, but I want to work on it, so I will. Feature request and improvement tips are welcome, but I want to keep the project very simple, keep that in mind.



Planned features
------------------

* Render colored points, lines and filled triangles positioned in 3D space (these are the primitives)
* SU::Model to bundle these primitives, also to save and load them via a file
* SU::Transformation to transform SU::Models (rotate, scale, mirror, translate, everything)
* SU::Object tree container to group and easily manipulate SU::Models (transform a group of them at once, etc.)
* Fast average distance-based shorting, optional Z-buffer
* Very basic lighting system
* ...



"Wishlist" features
-------------------

* Texturing
* Multiple backends (immediate OpenGL for example)
* ...



What is done
------------
* Since I just started this, almost nothing. But keep an eye on this project, I will likely progress fast (I'll have to finish it in time because I will present it)



Notes
-----

* The way to display an SDL_Surface with SDL2 is a bit complicated. You need an SDL_Window, an SDL_Renderer for that window, an SDL_Texture to render, and finally an SDL_Surface to create the texture from.
	It seems that the process of this takes up 9ms on my machine, no matter if I draw 1 single line on a surface or a hundred. I should find a faster way to display a single SDL_Surface in the window.


Licence
-------

Not yet chosen, likely zlib or CC-by.



Contact
-------
You can contact me here on GitHub or via email at iamsemmu AT gmail DOT com
