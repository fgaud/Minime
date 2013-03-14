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

/*
 * In this benchmark plugin, each worker thread/core performs
 * a sequence of (128-bit) memory writes instructions with
 * a sequential write pattern
 * (these writes bypass the cache and go directly to main memory)
 */

extern long WriterSSE2 (void *ptr, unsigned long loops, unsigned long size, unsigned long value);

#ifdef __x86_64__
uint64_t bench_throughput(uint64_t* memory_to_access, uint64_t memory_size, uint64_t time, uint32_t thread_no){
   return memory_size*WriterSSE2 (memory_to_access, time, memory_size, 0x1234567689abcdef);
}
#else
#error "This plugin only runs on x86_64 architectures"
#endif
