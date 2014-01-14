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
 * In this benchmark plugin, each worker thread/core performs
 * a sequence of (64-bit) memory load instructions on contiguous
 * memory locations inside a per-thread buffer
 */


#include "plugin_sequential_read.h"
#include <stdlib.h>
#include <string.h>

/*
 * Init phase:
 * Memset the array so that it is fully paged in memory before the bench
 */
void bench_seq_init(uint64_t *memory_to_access, uint64_t memory_size) {
   memset(memory_to_access, 0, memory_size);
}

/* 
 * Benchmark
 * The "O0" attribute is required to make sure that the compiler does not optimize the code
 */
uint64_t bench_seq_read(uint64_t* memory_to_access, uint64_t memory_size, uint64_t time, uint32_t thread_no) {
   int j, fake = 0;
   uint64_t start, current, nb_iterations = 0;
   rdtscll(start);

   while(1) {
      /*
       * The loop is partially unrolled in order to increase
       * the ratio of load instructions vs branch instructions
       */
      for (j = 0; j < (memory_size / sizeof(*memory_to_access)); j += 8) {
         fake += memory_to_access[j];
         fake += memory_to_access[j + 1];
         fake += memory_to_access[j + 2];
         fake += memory_to_access[j + 3];
         fake += memory_to_access[j + 4];
         fake += memory_to_access[j + 5];
         fake += memory_to_access[j + 6];
         fake += memory_to_access[j + 7];
      }

      nb_iterations++;
      rdtscll(current);
      if(current - start >= time)
         break;
   }

   return memory_size * nb_iterations;
}

