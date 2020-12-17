/*
 * main.ino -- main program for rc-car
 *
 * Copyright (C) 2020  Jacob Koziej <jacobkoziej@gmail.com>
 * Copyright (C) 2020  Matthew Bilik <matthewsings5@gmail.com>
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
	int8_t rot_ang;  // rotation angle + direction
	int16_t speed;   // user defined speed
	bool brake;      // brake or coast?
} usr_val;

struct motor_st {
	// index 0 is digital, index 1 is analog
	uint8_t l[2] = {4, 5};  // left motor
	uint8_t r[2] = {7, 6};  // right motor
} motor;

void setup()
{
	for (int i = 0; i < 2; i++) {
		pinMode(motor.l[i], OUTPUT);
		pinMode(motor.r[i], OUTPUT);
	}

	Serial.flush();
	Serial.begin(9600);
}

void loop()
{
	parse_usr_input(&usr_val);
	motor_drive(&motor, &usr_val);
}

void parse_usr_input(struct usr_val *in)
{
	while (1) {
		Serial.print("turning angle (+-90): ");
		while (!Serial.available());
		in->rot_ang = Serial.parseInt();
		Serial.println(in->rot_ang);
		if (90 >= in->rot_ang && -90 <= in->rot_ang) break;
	}

	while (1) {
		Serial.print("speed (+-255): ");
		while (!Serial.available());
		in->speed = Serial.parseInt();
		Serial.println(in->speed);
		if (255 >= in->speed && -255 <= in->speed) break;
	}

	if (in->speed) return;  // if non-zero speed, exit

	Serial.print("brake/coast (!0/0): ");
	while (!Serial.available());
	in->brake = Serial.parseInt() ? true : false;
	Serial.println(in->brake);
}

void motor_drive(struct motor_st *m, struct usr_val *u)
{
	// braking/coasting
	if (!u->speed) {
		if (u->brake) {
			digitalWrite(m->l[0], 1);
			digitalWrite(m->r[0], 1);
			digitalWrite(m->l[1], 1);
			digitalWrite(m->r[1], 1);
		} else {
			digitalWrite(m->l[0], 0);
			digitalWrite(m->r[0], 0);
			digitalWrite(m->l[1], 0);
			digitalWrite(m->r[1], 0);
		}

		return;  // exit if just braking/coasting
	}

	// get 8-bit values so it's nice to work with
	uint8_t master = abs(u->speed);
	uint8_t slave  = map(abs(u->rot_ang), 0, 90, master, 0);

	// decide which motor is master/slave
	uint8_t l_speed, r_speed;
	if (u->rot_ang < 0) {
		l_speed = master;
		r_speed = slave;
	} else {
		l_speed = slave;
		r_speed = master;
	}

	/*
	 * Since one of the motors is inverted to simplify wiring, bit-flipping
	 * the appropriate uint8_t will give us the correct complementary pwm
	 * value.
	 */
	if (u->speed > 0) {
		analogWrite(m->l[1], ~l_speed);
		analogWrite(m->r[1],  r_speed);
		digitalWrite(m->l[0], 1);
		digitalWrite(m->r[0], 0);
	} else {
		analogWrite(m->l[1],  l_speed);
		analogWrite(m->r[1], ~r_speed);
		digitalWrite(m->l[0], 0);
		digitalWrite(m->r[0], 1);
	}
}
