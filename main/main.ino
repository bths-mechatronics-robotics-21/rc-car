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
	uint8_t rot_ang;  // rotation angle + direction
	int16_t speed;    // user defined speed
} usr_val;

struct motor_st {
  uint8_t l[2] = {4, 5};  // left motor
  uint8_t r[2] = {6, 7};  // right motor
  float diamtr_in = 0.925;  // wheel diameter
  float width_in  = 5.191;  // chassis tire width
  float rpm = 100; // arbitrary RPM, TODO (change to match max)
} motor;

void setup()
{
  for (int i = 0; i < 2; i++) {
    pinMode(motor.l[i], OUTPUT);
    pinMode(motor.r[i], OUTPUT);
  }
	
  motor_drive(&motor, 128, 90 | _BV(8));
  Serial.flush();
  Serial.begin(9600);
}

void loop()
{
}

void parse_usr_input(struct usr_val *in)
{
	while (1) {
	Serial.print("turning angle (0-90): ");
	while (!Serial.available());
	in->rot_ang = Serial.parseInt();
	Serial.println(in->rot_ang);
	if (90 >= in->rot_ang) break;
	}

	Serial.print("left or right (0/1): ");
	while (!Serial.available());
	int buff = Serial.parseInt();
	Serial.println(buff);
	in->rot_ang |= _BV(buff ? 8 : 0);  // set direction bit

	while (1) {
	Serial.print("speed (+-255): ");
	while (!Serial.available());
	in->speed = Serial.parseInt();
	Serial.println(in->speed);
	if (255 >= in->speed && -255 <= in->speed) break;
	}
}

int angle_to_delay(struct motor_st *m, double rot_angle)
/* 
 * Delay as a function of RPM and angle. 
 */
{
  double millisPerRotation = (60 / m->rpm) * 1000;
  int anglePercentage = map(0, 360, 0, 1, rot_angle);
  return millisPerRotation * anglePercentage;
}

void motor_drive(struct motor_st *m, int16_t speed, uint8_t ctrl)
/*
 * uint8_t ctrl -- takes a value for the rotation angle from 0 to 90,
 * while bit 8 controls direction:
 *  0 - left
 *  1 - right
 */
{
  double rot_ang = ((ctrl & ~_BV(8)) * PI / 2) - (double)90;
  int delayVal = angle_to_delay(m, rot_ang);

  if (rot_ang == 0){
    if(speed > 0){
      // CW:
      digitalWrite(m->r[1], LOW);
      analogWrite(m->r[0], speed);
     
      digitalWrite(m->l[1], LOW);
      analogWrite(m->l[0], speed);
    } else {
      // CCW:
      digitalWrite(m->r[0], LOW);
      analogWrite(m->r[1], speed);
     
      digitalWrite(m->l[0], LOW);
      analogWrite(m->l[1], speed); 
    }
  }
  /* 
   * If angle > 0: turn accordingly based on direction. 
   * TODO: Jacob
   */
}
