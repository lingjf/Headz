#include "h2unit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "listz.h"
#include "hashz.h"


extern "C" {
unsigned long __next_prime(unsigned long __n);
}


H2SUITE(hasht)
{ 

	void setup()
  	{ 
  	}

  	void teardown()
  	{  		
  	}  	  	
} ;

H2CASE(hasht, __next_prime_from_prime)
{
	H2EQUAL_INTEGER(97ul,__next_prime(53ul));
	H2EQUAL_INTEGER(193ul,__next_prime(97ul));
	H2EQUAL_INTEGER(389ul,__next_prime(193ul));
	H2EQUAL_INTEGER(769ul,__next_prime(389ul));
	H2EQUAL_INTEGER(1543ul,__next_prime(769ul));
	H2EQUAL_INTEGER(3079ul,__next_prime(1543ul));
	H2EQUAL_INTEGER(6151ul,__next_prime(3079ul));
	H2EQUAL_INTEGER(12289ul,__next_prime(6151ul));
	H2EQUAL_INTEGER(24593ul,__next_prime(12289ul));
	H2EQUAL_INTEGER(49157ul,__next_prime(24593ul));
	H2EQUAL_INTEGER(98317ul,__next_prime(49157ul));
	H2EQUAL_INTEGER(196613ul,__next_prime(98317ul));
	H2EQUAL_INTEGER(393241ul,__next_prime(196613ul));
	H2EQUAL_INTEGER(786433ul,__next_prime(393241ul));
	H2EQUAL_INTEGER(1572869ul,__next_prime(786433ul));
	H2EQUAL_INTEGER(3145739ul,__next_prime(1572869ul));
	H2EQUAL_INTEGER(6291469ul,__next_prime(3145739ul));
	H2EQUAL_INTEGER(12582917ul,__next_prime(6291469ul));
	H2EQUAL_INTEGER(25165843ul,__next_prime(12582917ul));
	H2EQUAL_INTEGER(50331653ul,__next_prime(25165843ul));
	H2EQUAL_INTEGER(100663319ul,__next_prime(50331653ul));
	H2EQUAL_INTEGER(201326611ul,__next_prime(100663319ul));
	H2EQUAL_INTEGER(402653189ul,__next_prime(201326611ul));
	H2EQUAL_INTEGER(1610612741ul,__next_prime(805306457ul));
	H2EQUAL_INTEGER(3221225473ul,__next_prime(1610612741ul));
	H2EQUAL_INTEGER(4294967291ul,__next_prime(3221225473ul));
	H2EQUAL_INTEGER(0,__next_prime(4294967291ul));

}

H2CASE(hasht, __next_prime_from_normal)
{
	H2EQUAL_INTEGER(53ul,__next_prime(13));
	H2EQUAL_INTEGER(97ul,__next_prime(73ul));
	H2EQUAL_INTEGER(193ul,__next_prime(107ul));
	H2EQUAL_INTEGER(389ul,__next_prime(293ul));
	H2EQUAL_INTEGER(769ul,__next_prime(399ul));
	H2EQUAL_INTEGER(1543ul,__next_prime(799ul));
	H2EQUAL_INTEGER(3079ul,__next_prime(1593ul));
	H2EQUAL_INTEGER(6151ul,__next_prime(3979ul));
	H2EQUAL_INTEGER(12289ul,__next_prime(6951ul));
	H2EQUAL_INTEGER(24593ul,__next_prime(19289ul));
	H2EQUAL_INTEGER(49157ul,__next_prime(29593ul));
	H2EQUAL_INTEGER(98317ul,__next_prime(49957ul));
	H2EQUAL_INTEGER(196613ul,__next_prime(98917ul));
	H2EQUAL_INTEGER(393241ul,__next_prime(196913ul));
	H2EQUAL_INTEGER(786433ul,__next_prime(393941ul));
	H2EQUAL_INTEGER(1572869ul,__next_prime(789433ul));
	H2EQUAL_INTEGER(3145739ul,__next_prime(1972869ul));
	H2EQUAL_INTEGER(6291469ul,__next_prime(3945739ul));
	H2EQUAL_INTEGER(12582917ul,__next_prime(6299469ul));
	H2EQUAL_INTEGER(25165843ul,__next_prime(12592917ul));
	H2EQUAL_INTEGER(50331653ul,__next_prime(25195843ul));
	H2EQUAL_INTEGER(100663319ul,__next_prime(50931653ul));
	H2EQUAL_INTEGER(201326611ul,__next_prime(190663319ul));
	H2EQUAL_INTEGER(402653189ul,__next_prime(291326611ul));
	H2EQUAL_INTEGER(1610612741ul,__next_prime(895306457ul));
	H2EQUAL_INTEGER(3221225473ul,__next_prime(1910612741ul));
	H2EQUAL_INTEGER(4294967291ul,__next_prime(3941225473ul));
}


