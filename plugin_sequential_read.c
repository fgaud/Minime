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

#include "plugin_sequential_read.h"
#include <stdlib.h>

void bench_seq_init(uint64_t *memory_to_access, uint64_t memory_size) {
   int i;
   unsigned int seed = 0;
   for (i = 0; i < memory_size / sizeof(*memory_to_access); i++) {
      memory_to_access[i] = rand_r(&seed);
   }
}

__attribute__((optimize("O0")))   uint64_t bench_seq_read(uint64_t* memory_to_access, uint64_t memory_size, uint32_t nb_iterations, uint32_t thread_no) {
   int i, j, fake = 0;

   for (i = 0; i < nb_iterations; i++) {
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
   }

   return memory_size * nb_iterations;
}

