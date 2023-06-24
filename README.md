# Minute to Guess it: Binary Number Edition

![The Completed Project](/project_images/final_design.png)

The "Minute to Guess it: Binary Edition" is a game where the player has 60 seconds to guess the decimal equivalent of a binary number displayed on an LCD. The game provides a fun and challenging way to test your binary conversion skills.

## Features

- Timers
- Debouncing
- Interrupts
- Registers
- LEDs indicate the remaining time in 15-second intervals.
- The frequency of the buzzer sound indicates the correctness of the user's answer and if the time is up.

## Images

<figure>
  <img src="/project_images/intro_screen.png" alt="LCD Screen Intro State">
  <figcaption>Figure 1: Intro Screen</figcaption>
</figure>

<br> <!-- Add a line break here -->

<figure>
  <img src="/project_images/guess_screen.png" alt="LCD Screen Guess State">
  <figcaption>Figure 2: Guessing Screen</figcaption>
</figure>

<br> <!-- Add a line break here -->

<figure>
  <img src="/project_images/game_over_screen.png" alt="LCD Screen Game Over State">
  <figcaption>Figure 3: Game Over Screen</figcaption>
</figure>

<br> <!-- Add a line break here -->

<figure>
  <img src="/project_images/led.png" alt="LEDs">
  <figcaption>Figure 4: LEDs that indicate the amount of time remaining</figcaption>
</figure>


## Hardware Components

- Arduino Uno board
- LCD display (I2C)
- Keypad
- 4 x 220 ohm resistors
- 4 LEDs
- Active buzzer
- Push button

## How to Play

1. Press the button to start the game.
2. The binary number will be displayed on the LCD screen.
3. Use the keypad to input your guess in decimal form.
4. If the guess is correct, you will hear a short high pitched beep and score 2 points.
5. If the guess is wrong, you will hear a short low pitched beep and lose 1 point.
6. The LEDs indicate the remaining time in 15-second intervals.
7. As the time progresses, the LEDs turn off one at a time in 15-second intervals untill the 60 seconds are up.
8. If the time runs out, you will hear long high pitched beep.
9. After the game ends, the stats will be displayed.

## Note from the Developer

I'm relatively new to programming microcontrollers, timer interrupts, and button interrupts. This project has been a great learning experience for me, and I wanted to create something that could serve as a learning resource for others who are also new to these concepts.

While the code provided here is functional and should work, it's always a good idea to further explore and experiment with the code to deepen your understanding.
I hope this project helps you on your journey of learning microcontroller programming and encourages you to explore further possibilities. If you have any suggestions, improvements, or new features to add, feel free to open an issue or submit a pull request.

