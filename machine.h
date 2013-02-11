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

#ifndef MACHINE_H_
#define MACHINE_H_

#ifdef __x86_64__
#define rdtscll(val) { \
	    unsigned int __a,__d;                                        \
	    asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
	    (val) = ((unsigned long)__a) | (((unsigned long)__d)<<32);   \
}
#else
   #define rdtscll(val) __asm__ __volatile__("rdtsc" : "=A" (val))
#endif

#endif /* MACHINE_H_ */
