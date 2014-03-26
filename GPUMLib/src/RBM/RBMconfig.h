/*
	Noel Lopes is an Assistant Professor at the Polytechnic Institute of Guarda, Portugal
	Copyright (C) 2009, 2010, 2011, 2012 Noel de Jesus Mendon�a Lopes

	This file is part of GPUMLib.

	GPUMLib is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GPUMLib_RBMconfig_h
#define GPUMLib_RBMconfig_h

#define U_FACTOR (CUDA_VALUE(1.2))
#define D_FACTOR (CUDA_VALUE(0.8))
#define MAX_STEP_SIZE (CUDA_VALUE(10.0))

#define INITIAL_BIAS_HIDDEN_UNITS (CUDA_VALUE(-4.0))
#define STD_WEIGHTS (CUDA_VALUE(0.01))
#define DEFAULT_MOMENTUM (CUDA_VALUE(0.3))

#include "../common/CudaDefinitions.h"
#include "../memory/HostMatrix.h"

#endif