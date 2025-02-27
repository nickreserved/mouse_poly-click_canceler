The program, every time you click the mouse, ignores any further clicks produced by the mouse due to malfunctioning key switches.

It handles the right and left mouse buttons only.

It works as follows:
- If a mouse-button-release event comes, then:
  - It cancels the event.
  - It cancels any active timer (see below) without performing the action assigned to it.
  - It sets a timer to 50msec and once the time has passed, it sends the previously canceled event to Windows.
- If a mouse-button-press event comes, then:
  - If there is an active timer, then:
    - It cancels it. (Anyway, after the button is pressed, another mouse-button-release event will appear shortly that will reactivate the timer).
    - Cancels the event (It is very close (< 50msec) in time to a previous mouse-button-release event, so it is unlikely to have been caused by a human - it is most likely due to a malfunctioning mouse).
  - If there is no active timer, then the event is not canceled and processed normally from Windows.

The source code runs on Windows 95 and later.<br>
The binaries probably not.<br>
If there is an interest for binaries for Windows 95, send me an email.
