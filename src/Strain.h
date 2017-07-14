/*
Strain.h - 
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

#ifndef Strain_h
#define Strain_h

#include <inttypes.h>

#define PROOF_MASS 365 // This is approx. the mass of a full can of beer or pop in grams

class Strain
{
public:
	Strain(int filter_size);
	Strain(int filter_size, int cal_addr);
	Strain(int filter_size, uint16_t* filter_vector, int cal_addr);

	void attach(int pin);
	void calibrate(int cal_addr);

	uint16_t nextSample();
	uint16_t read();

	// both are recalculated on each call to nextSample()
	int32_t mean, variance;
private:
	uint16_t* readings;
	uint16_t  reading_count;
	uint16_t  r_idx;

	uint16_t cal;
	uint8_t  pin;

	void     init_filter(int size);
};

#endif
