#include <string.h>

#define isSame(x,y)  (!strncmp ((x),(y),strlen((x))))

void defineFeature (int *v, int *m, int *p, char *token)
{
/* 
	This feature allocation is quite arbitrary, may need to change if necessary.
	The "manner" category is defined arbitrarily. (Unlike the voicing and place 
	categories, where you can meaningfully define the feature in a binrary or 
	tertiary way, the place category is troublesome to define in a quaternary way 
	(i.e., what exactly do you mean by manner 0 feature versus manner 3 feature?) 

    A binary or tertiary classification of place features may be meaningful only if
	a small set of "frequently-used" phonemes are used for testing, small enough
	to make the derived features of place two (e.g., stops and fricatives). If the
	number of place features is equal or more than three, the problem begins.
	For example, a nasal consonant could be considered as a stop consonant, if located
	in an syllable-initial position. Should I separate nasals from the other and make 
	another classification of nasals, or what about glides? what about lateral? Or
	how about affricates? Should they be in the same category as stops or fricatives or 
	their own? The question becomes endless. 

	Therefore, if one is interested in analyzing place features of articulation, it is 
	more prudent to use sub-category classification (such as strident, nasal, duration, etc)
	each of which has only binarary (reluctantly tertiary) category. 

    But who cares in this business? I am just making it as "place" and report it that way
	the general audience in this world probably doesn't even understand what strident is.


  Bomjun Kwon. Feb 10, 2004.


Character code:

th  -  three
thv -  they
j   -  jelly
zh  -  measure

  
voicing
b d g m n ng v thv z zh j l r 
p t k f th f s sh ch h

  manner
b d g p t k 
m n ng
v f thv th z s zh sh j ch h
l r w y

  place
m b p v f w
n d t thv th l z s y
g k zh sh j ch r h

  */

	if isSame ((token), ("b"))
	{		*v=0; *m=0; *p=0; return; 	}
	else if isSame (token, "d")
	{		*v=0; *m=0; *p=1; return;	}
	else if isSame (token, "g")
	{		*v=0; *m=0; *p=2; return;	}
	else if isSame (token, "p")
	{		*v=1; *m=0; *p=0; return;	}
	else if isSame (token, "t")
	{		*v=1; *m=0; *p=1; return;	}
	else if isSame (token, "k")
	{		*v=1; *m=0; *p=2; return;	}
	else if isSame (token, "m")
	{		*v=0; *m=1; *p=0; return;	}
	else if isSame (token, "n")
	{		*v=0; *m=1; *p=1; return;	}
	else if isSame (token, "ng")
	{		*v=0; *m=1; *p=2; return;	}
	else if isSame (token, "v")
	{		*v=0; *m=2; *p=0; return;	}
	else if isSame (token, "z")
	{		*v=0; *m=2; *p=1; return;	}
	else if isSame (token, "zh")
	{		*v=0; *m=2; *p=2; return;	}
	else if isSame (token, "j")
	{		*v=0; *m=2; *p=2; return;	}
	else if isSame (token, "l")
	{		*v=0; *m=3; *p=1; return;	}
	else if isSame (token, "r")
	{		*v=0; *m=3; *p=2; return;	}
	else if isSame (token, "f")
	{		*v=1; *m=2; *p=0; return;	}
	else if isSame (token, "s")
	{		*v=1; *m=2; *p=1; return;	}
	else if isSame (token, "sh")
	{		*v=1; *m=2; *p=2; return;	}
	else if isSame (token, "ch")
	{		*v=1; *m=2; *p=2; return;	}
	else if isSame (token, "th")
	{		*v=1; *m=2; *p=2; return;	}
	else if isSame (token, "dh")
	{		*v=0; *m=2; *p=2; return;	}
	else if isSame (token, "w")
	{		*v=0; *m=3; *p=0; return;	}
	else if isSame (token, "y")
	{		*v=0; *m=3; *p=1; return;	}
	else if isSame (token, "h")
	{		*v=1; *m=2; *p=2; return;	}
	else 
	{		*v=-1; *m=-1; *p=-1; return;	}
}

