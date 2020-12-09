/*
 * main.ino -- main program for rc-car
 *
 * Copyright (C) 2020  Jacob Koziej <jacobkoziej@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

struct usr_val {
	int rot_ang;    // rotation angle + direction
	int16_t speed;  // user defined speed
} usr_val;

void setup()
{
	Serial.flush();
	Serial.begin(9600);
}

void loop()
{
}

void parse_usr_input(struct usr_val *in)
{
	Serial.print("\nturning angle (0-90): ");
	while (!Serial.available());
	in->rot_ang = Serial.parseInt();
	Serial.println(in->rot_ang);

	Serial.print("left or right (0/1): ");
	while (!Serial.available());
	int buff = Serial.parseInt();
	Serial.println(buff);
	in->rot_ang |= _BV(buff ? 8 : 0);  // set direction bit
	Serial.println(in->rot_ang);

	Serial.print("speed (+-255): ");
	while (!Serial.available());
	in->speed = Serial.parseInt();
	Serial.print(in->speed);
}
