# Driver qtpy (SDI-12)

This device driver is called qtpy for the Adafruit QT Py board, since that's
what we use in production. But any board running the lagopus sketch from the
wsn\_arduino repository will do.

The sensors are attached to the board running the lagopus sketch, so we call
it the *sensor board*.

The main board running this program, and the sensor board running the lagopus
sketch, communicate through the SDI-12 protocol.
