# Bike Sensor

A suite of programs for capturing data from an exercise bike.

## Programs

### Arduino

The first item in this suite is an Arduino program for reading pulses
from an exercise bike.  It was designed for the [FitDesk v2.0 Desk
Exercise Bike](fitdesk) because that is the bike that I have, but it
should be fairly easy to make it work with any other bike that uses a
similar method to track revolutions.  It'd probably be pretty easy to
implement on a normal bike with a trainer as well.

The FitDesk uses a 3.5mm headphone connector which has its single
channel shorted to ground once per revolution.  The controller reads in
those pulses and uses them to calculate the RPM and send it to the host
computer over serial.

### Console

The second part of the project is a text based script that reads in the
RPM date from the serial device and displays useful information such as
your speed and distance.  This is effectively a replacement for the
little display that came with the FitDesk.

### GUI

The last part of the suite is a GUI application to do much the same
stuff as the console version.  This is not yet implemented.

## License

This code is licensed under the MIT license.  You may use it as you see
fit, as long as the copyright notice is maintained.  See the
[LICENSE](license) file for details.

[fitdesk]: http://www.amazon.com/gp/product/B00CM9CBZY/ref=as_li_tl?ie=UTF8&camp=1789&creative=9325&creativeASIN=B00CM9CBZY&linkCode=as2&tag=eatabrick07-20&linkId=FDIVQ7JAOKBK75WD
[license]: https://www.github.com/bentglasstube/bike-sensor/tree/master/LICENSE
