<h1>DspHexDisplay</h1>

This is the C code of my Master's thesis.

Project goal was to output a video signal into a hexagonal display matrix and to explore antialiasing algorithms on that display.

It was achieved by utilizing Analog Devices BlackFin 533 DSP processor "ADSP-BF533" test module.

In the code you can see some device settings management in form of sending specific numbers to certain addresses. Each bit in the address corresponds to a certain setting being on or off or in some cases several of those bits combining to a number value.
There's also some logic statements, loops and array manipulation.

Fun facts about the project:
<ul>
<li>the module had 80px</li>
<li>each pixel had 18bit color depth</li>
<li>module had a refresh rate of 1785 Hz</li>
<li>a undocumented processor bug was found related to SPI DMA interrupt flags</li>
</ul>
