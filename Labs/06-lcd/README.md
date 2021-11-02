# Vypracovanie LAB_6 - Alexander Bekeč

Alexander Bekeč, 221096

Link to depository: [https://github.com/alexander-bekec/Digital-electronics-2](https://github.com/alexander-bekec/Digital-electronics-2)

### LCD display module

1. In your words, describe what ASCII table is.
   * ASCII table: Tabuľka pre kódovanie znakov do binárnej podoby (netlačené znaky, tlačené znaky - písmená malé, písmená veľké, číslice, špecialné symboly, interpunkčné znamienka)

2. (Hand-drawn) picture of time signals between ATmega328P and LCD keypad shield (HD44780 driver) when transmitting three character data `De2`.

   ![Signál](IMAGES\00_signal_De2.png)


### Stopwatch

1. Flowchart figure for `TIMER2_OVF_vect` interrupt service routine which overflows every 16&nbsp;ms but it updates the stopwatch LCD approximately every 100&nbsp;ms (6 x 16&nbsp;ms = 100&nbsp;ms). Display tenths of a second and seconds `00:seconds.tenths`. Let the stopwatch counts from `00:00.0` to `00:59.9` and then starts again. The image can be drawn on a computer or by hand. Use clear descriptions of the individual steps of the algorithms.

   ![Diagram](IMAGES\00_overflowchart.png)


### Custom characters

1. Code listing of two custom character definition. Always use syntax highlighting and meaningful comments:

```c
/* Variables ---------------------------------------------------------*/
// Custom character definition
uint8_t customChar[16] = {
    // EURO CURRENCY SIGN
    0b00110,
    0b01001,
    0b11100,
    0b01000,
    0b11100,
    0b01001,
    0b00110,
    0b00000,
    // LINE IN FIRST COLUMN
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000,
    0b10000
};
```

### Kitchen alarm

Consider a kitchen alarm with an LCD, one LED and three push buttons: start, +1 minute, -1 minute. Use the +1/-1 minute buttons to increment/decrement the timer value. After pressing the Start button, the countdown starts. The countdown value is shown on the display in the form of mm.ss (minutes.seconds). At the end of the countdown, the LED will start blinking.

1. Scheme of kitchen alarm; do not forget the supply voltage. The image can be drawn on a computer or by hand. Always name all components and their values.

   ![Schéma zapojenia](IMAGES\00_minutka_schema.png)