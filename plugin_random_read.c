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

#include "plugin_random_read.h"
#include <stdlib.h>
#include <assert.h>

#define RANDOM_BECOMES_SEQUENTIAL 0

struct ij {
   int i;
   int j;
};

static int compar(const void* a1, const void* a2) {
   struct ij *a, *b;
   a = (struct ij*) a1;
   b = (struct ij*) a2;
   return a->j - b->j;
}

void bench_rand_read_init(uint64_t *memory_to_access, uint64_t memory_size) {
   int i;
   unsigned int seed = 1;

   int array_size = memory_size / sizeof(*memory_to_access);

   struct ij *rand_array = malloc(sizeof(struct ij) * array_size);
   assert(rand_array);

   for (i = 1; i < array_size; i++) {
      rand_array[i].i = i;
#if RANDOM_BECOMES_SEQUENTIAL
      rand_array[i].j = i;
#else
      rand_array[i].j = rand_r(&seed);
#endif
   }
   qsort(&rand_array[1], array_size - 1, sizeof(*rand_array), compar);

   uint64_t *addr = (uint64_t*) memory_to_access;
   for (i = 0; i < array_size - 1; i++) {
      //* addr = (uint64_t) &buf[rand_array[i + 1].i * stride];
      *addr = (uint64_t) &memory_to_access[rand_array[i + 1].i];
      addr = (uint64_t*) *addr;
   }
   *addr = (uint64_t) &memory_to_access[0]; /** Be sure to loop **/

   free(rand_array);
}

__attribute__((optimize("O0")))  uint64_t bench_rand_read(uint64_t* memory_to_access, uint64_t memory_size, uint64_t time, uint32_t thread_no) {

   uint64_t start, current, nb_iterations = 0;
   rdtscll(start);
   
   while(1) {
      uint64_t rest = memory_size / sizeof(*memory_to_access);
      uint64_t *addr = *((uint64_t**) memory_to_access);

      while (rest) {
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;

         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;

         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;

         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;
         addr = (uint64_t *) *addr;

         rest -= 64;
      }

      nb_iterations++;
      rdtscll(current);
      if(current - start >= time)
         break;
   }

   return memory_size * nb_iterations;
}

