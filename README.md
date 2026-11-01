**Trajectory and Orbit Calculator**

A command-line program written in C that simulates projectile motion with a numerical method and calculates the parameters of a circular orbit around the Earth.

**Features**
Simulates a projectile launched at any speed, angle and starting height
Uses a time-stepped numerical method and compares the result with the exact physics formulas
Prints a table of time, position and speed during the flight
Saves the flight path to trajectory.csv so it can be plotted in Excel or Python
Calculates orbital velocity, period, escape velocity and orbits per day for a given altitude
Checks all input and asks again when a value is invalid
How it works
**Projectile motion**

The flight is simulated in small time steps (dt) using the semi-implicit Euler method. In every step:

vy = vy - g * dt
x  = x + vx * dt
y  = y + vy * dt
t  = t + dt

When the projectile drops below the ground, the exact landing point is found by linear interpolation between the last two steps.

The results are then compared with the analytical formulas:

max height   = h0 + (v0 sin a)^2 / (2g)
flight time  = (v0 sin a + sqrt((v0 sin a)^2 + 2 g h0)) / g
range        = v0 cos a * flight time

The program prints the percentage error between the simulation and the formulas. A smaller time step gives a smaller error.

**Circular orbit**

For an altitude h, the orbit radius is r = R_earth + h. The program then calculates:

orbital velocity = sqrt(GM / r)
orbital period   = 2 * pi * r / v
escape velocity  = sqrt(2 * GM / r)

Constants used: g = 9.80665 m/s^2, GM = 3.986004418e14 m^3/s^2, Earth radius = 6371 km.

**Build and run**

You need a C compiler such as gcc.

gcc Trajectory.c -o Trajectory.exe -lm
.\Trajectory.exe

On Linux or macOS:

gcc Trajectory.c -o trajectory -lm
./trajectory
Input ranges
Input	Range
Initial speed	1 to 1000 m/s
Launch angle	1 to 90 degrees
Initial height	0 to 10000 m
Time step	0.0001 to 0.1 s
Orbit altitude	100 to 400000 km
Output file

Each projectile run creates trajectory.csv in the folder where the program is run. It has three columns: time_s, x_m and y_m.

**Limitations**
The projectile model has constant gravity and no air resistance
Only circular orbits are calculated, and only around the Earth
The Earth is treated as a perfect sphere with a single average radius
It is a learning project and is not meant for real mission planning

**Possible improvements**
Add air resistance (drag) to the projectile model
Add elliptical orbits
Let the user choose another planet
Plot the trajectory directly with a plotting library

**Concepts used**
C programming, functions, structured input validation, file handling, numerical integration (Euler method), error analysis, basic orbital mechanics.
