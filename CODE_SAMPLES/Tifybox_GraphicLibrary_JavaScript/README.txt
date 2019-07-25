
                _._     _,-'""`-._
              (,-.`._,'(       |\`-/|
                  `-.-' \ )-`( , o o)
                        `-    \`_`"'-       LaserChaser.org



TifyBox was developed as a web applicaiton using the Laravel 
framework. The main attraction of the webiste is the "tifyBox",
which is a 3-d cube which the user can spin, translate, and 
pick individual squares on the surface. Each square is mapped
to a YouTube site. The idea is to have a random web surfing 
experience, to see videos which the user would otherwise not
see. The jewel of this project was the developement of the 
box's graphics and physics. WebGL was used as a basis, but most
of the heavy lifting was done by your's truly. 

Of special interest is the pixel selection handler, which translates
a selection made in 2-d space toa coordinate on the cube in 3-d 
space. Vector and Fragment shaders are used to handle this magic. :)

Sample Website:
https://tifybox.laserchaser.org/

