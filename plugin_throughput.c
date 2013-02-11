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
* This plugin uses the ASM code of bandwidth (0.16), available here : http://zsmith.co/bandwidth.html
* This program is free software, published under the terms of the GNU General 
* Public License either version 2 of the License, or any later version.
*/

#include "memory_test.h"
#include "plugin_throughput.h"

extern int WriterSSE2 (void *ptr, unsigned long loops, unsigned long size, unsigned long value);

#ifdef __x86_64__
uint64_t bench_throughtput(uint64_t* memory_to_access, uint64_t memory_size, uint32_t nb_iterations, uint32_t thread_no){
   WriterSSE2 (memory_to_access, nb_iterations, memory_size, 0x1234567689abcdef);
   return memory_size*nb_iterations;
}
#else
#error "This plugin only runs on x86_64 architectures"
#endif
