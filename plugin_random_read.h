/*
Copyright (C) 2012  
Fabien Gaud <fgaud@sfu.ca>, Baptiste Lepers <baptiste.lepers@inria.fr>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
This plugin is inspired by the Corey benchmark.
Original source code is available here: http://pdos.csail.mit.edu/corey/
*/

#ifndef PLUGIN_RANDOM_READ_H_
#define PLUGIN_RANDOM_READ_H_

#include <stdint.h>
#include "machine.h"

void bench_rand_read_init(uint64_t *memory_to_access, uint64_t memory_size);
uint64_t bench_rand_read(uint64_t* memory_to_access, uint64_t memory_size, uint64_t time, uint32_t thread_no);

#endif /* PLUGIN_RANDOM_READ_H_ */
