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

#ifndef PLUGIN_SEQUENTIAL_READ_H_
#define PLUGIN_SEQUENTIAL_READ_H_

#include <stdint.h>

void bench_seq_init(uint64_t *memory_to_access, uint64_t memory_size);
uint64_t bench_seq_read(uint64_t* memory_to_access, uint64_t memory_size, uint32_t nb_iterations, uint32_t thread_no);

#endif /* PLUGIN_SEQUENTIAL_READ_H_ */
