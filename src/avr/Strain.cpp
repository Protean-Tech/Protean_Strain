/*
Copyright (c) 2017 Kirk Roerig.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3.0 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/



#include <Arduino.h>
#include <EEPROM.h>

#include "Strain.h"

static uint16_t eeprom_read16(int addr)
{
	uint16_t i;
	uint8_t* i_ptr = (uint8_t*)&i;

	i_ptr[0] = EEPROM.read(addr + 0);
	i_ptr[1] = EEPROM.read(addr + 1);

	return i;
}


static void eeprom_write16(int addr, uint16_t i)
{
	uint8_t* i_ptr = (uint8_t*)&i;

	EEPROM.write(addr + 0, i_ptr[0]);
	EEPROM.write(addr + 1, i_ptr[1]);
}


Strain::Strain(int size)
{
	init_filter(size);

	cal = 1;
}


Strain::Strain(int size, int cal_addr)
{
	init_filter(size);

	cal = eeprom_read16(cal_addr);
}


Strain::Strain(int size, uint16_t* filter_vector, int cal_addr)
{
	readings      = filter_vector;
	reading_count = size;
	r_idx         = 0;

	cal = eeprom_read16(cal_addr);
}


void Strain::init_filter(int size)
{
	readings      = (uint16_t*)malloc(sizeof(uint16_t) * size);
	reading_count = size;
	r_idx         = 0;
}


void Strain::attach(int pin)
{
	this->pin = pin;
}


void Strain::calibrate(int cal_addr)
{
	cal = PROOF_MASS / analogRead(pin);
	eeprom_write16(cal_addr, cal);
}


uint16_t Strain::nextSample()
{
	uint16_t sample = readings[r_idx++] = analogRead(pin);
	r_idx %= reading_count;

	// compute the mean
	mean = 0;
	for(int i = reading_count; i--;)
	{
		mean += readings[i];
	}
	mean /= reading_count;

	// compute the variance
	variance = 0;
	for(int i = reading_count; i--;)
	{
		int16_t delta = mean - (int16_t)(readings[i]);
		variance += (delta * delta);
	}
	variance /= reading_count;

	return sample;
}


uint16_t Strain::read()
{
	int std_dev = sqrt(variance);
	int counted = 0;
	int filtered = 0;

	// Simple statistical filter to try to reduce noise
	for(int i = reading_count; i--;)
	{
		int16_t delta = mean - (int16_t)(readings[i]);

		if(delta <= std_dev)
		{
			++counted;
			filtered += readings[i];
		}
	}

	filtered /= counted;

	return filtered * cal;
}
