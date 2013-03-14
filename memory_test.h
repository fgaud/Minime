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

#ifndef MEMORY_TEST_H_
#define MEMORY_TEST_H_

/** Plugins **/
#include "plugin_throughput.h"
#include "plugin_sequential_read.h"
#include "plugin_random_read.h"

uint64_t bench_memory(uint64_t* memory_to_access, uint64_t memory_size, uint64_t time, uint32_t thread_no);
void stride_write_buf(uint64_t *buf, uint64_t size);

typedef uint64_t (*bench_memory_fun_t)(uint64_t*, uint64_t, uint64_t, uint32_t);
typedef void (*init_memory_fun_t)(uint64_t*, uint64_t);

typedef struct {
      const char * name; /* plugin name */
      
      /*
       * Initialization function (called by each worker thread)
       * Args:
       * - buffer
       * - buffer size
       */
      init_memory_fun_t init_fun;
      
      /*
       * Benchmarking function (called by each worker thread)
       * Args:
       * - buffer
       * - buffer size
       * - time (duration of the benchmark in cycles)
       * - thread number/id
       * Return value: number of accessed bytes
       */    
      bench_memory_fun_t bench_fun;
} memory_bench_plugin_t;

#define die(msg, args...) \
do {                         \
            fprintf(stderr,"(%s,%d) " msg "\n", __FUNCTION__ , __LINE__, ##args); \
            exit(-1);                 \
         } while(0)

#endif //MEMORY_TEST_H_
