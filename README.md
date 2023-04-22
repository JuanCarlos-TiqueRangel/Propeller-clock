## Propeller clock with PIC16F877A

This is a simple program written in C language for the PIC microcontroller, specifically the 16F877A

# Hardware requirements
* Pic microncontroller, specifically the PIC16f877A
* leds
* Lipo battery 7.4v
* Resistors
* Wires
* PIC KIT 3

# Software requirements
* MPLAB or any other IDE for C programming
* CCS or any other compiler for PIC microcontrollers

# How to use
Connect the PIC microcontroller to designed board. Open the project in your preferred IDE and compile the code.
Flash the compiled code onto the PIC microcontroller using PICKIT or any other software.

# Files
[POV.c](POV.c)

# License
This code is released under the MIT License. Feel free to modify and use it in your own projects.

```bash
# Your bash code here
```
<button class="btn" onclick="copyToClipboard(event)">Copy</button>

<script>
  function copyToClipboard(event) {
    const button = event.target;
    const code = button.parentNode.querySelector('code');
    const text = code.innerText;
    navigator.clipboard.writeText(text).then(() => {
      button.innerText = 'Copied!';
      setTimeout(() => {
        button.innerText = 'Copy';
      }, 2000);
    });
  }
</script>
