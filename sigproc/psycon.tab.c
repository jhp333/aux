
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 2 "psycon.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psycon.yacc.h"
#define strdup _strdup
#define YYPRINT(file, type, value) print_token_value (file, type, value)
/*#define DEBUG*/

char *ErrorMsg = NULL;
int yylex (void);
void yyerror (AstNode **pproot, char **errmsg, char const *s);


/* Line 189 of yacc.c  */
#line 88 "psycon.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 1
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_EOF = 0,
     T_UNKNOWN = 258,
     T_NEWLINE = 259,
     T_IF = 260,
     T_ELSE = 261,
     T_ELSEIF = 262,
     T_END = 263,
     T_WHILE = 264,
     T_FOR = 265,
     T_BREAK = 266,
     T_CONTINUE = 267,
     T_SWITCH = 268,
     T_CASE = 269,
     T_OTHERWISE = 270,
     T_FUNCTION = 271,
     T_ENDFUNCTION = 272,
     T_RETURN = 273,
     T_SIGMA = 274,
     T_OP_SHIFT = 275,
     T_OP_SHIFT2 = 276,
     T_OP_CONCAT = 277,
     T_COMP_EQ = 278,
     T_COMP_NE = 279,
     T_COMP_LE = 280,
     T_COMP_GE = 281,
     T_LOGIC_AND = 282,
     T_LOGIC_OR = 283,
     T_NUMBER = 284,
     T_STRING = 285,
     T_ID = 286,
     T_NEGATIVE = 287,
     T_POSITIVE = 288,
     T_LOGIC_NOT = 289
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 23 "psycon.y"

	double dval;
	char *str;
	AstNode *pnode;



/* Line 214 of yacc.c  */
#line 167 "psycon.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 99 "psycon.y"

AstNode *newAstNode(int type, YYLTYPE loc);
AstNode *makeFunctionCall(char *name, AstNode *first, AstNode *second, YYLTYPE loc);
AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc);
void print_token_value(FILE *file, int type, YYSTYPE value);


/* Line 264 of yacc.c  */
#line 200 "psycon.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  57
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1308

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  57
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNRULES -- Number of states.  */
#define YYNSTATES  250

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   289

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    55,     2,    38,     2,     2,
      48,    49,    40,    37,    46,    36,     2,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    35,    47,
      33,    32,    34,     2,    39,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    42,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    50,    56,    51,    54,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    43,    44,    45
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     6,     8,    11,    13,    16,    18,
      21,    24,    26,    28,    30,    32,    34,    36,    37,    39,
      41,    43,    45,    47,    53,    61,    66,    73,    78,    85,
      87,    89,    91,    99,   109,   121,   133,   134,   139,   140,
     142,   148,   155,   159,   163,   167,   171,   175,   179,   183,
     186,   190,   194,   195,   197,   201,   205,   211,   213,   215,
     217,   221,   222,   224,   228,   230,   233,   237,   241,   247,
     249,   251,   255,   262,   269,   279,   283,   290,   297,   307,
     311,   320,   329,   331,   333,   335,   340,   348,   352,   356,
     361,   366,   372,   379,   382,   385,   389,   398,   402,   406,
     410,   414,   418,   422,   426,   432,   436,   440,   444,   448,
     452
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      58,     0,    -1,    -1,    59,    -1,    62,    -1,    59,    62,
      -1,    61,    -1,    60,    61,    -1,     4,    -1,     1,     4,
      -1,    65,    63,    -1,    61,    -1,    66,    -1,    46,    -1,
      47,    -1,     4,    -1,     0,    -1,    -1,    17,    -1,     0,
      -1,    78,    -1,    77,    -1,    69,    -1,     5,    69,    60,
      67,     8,    -1,     5,    69,    60,    67,     6,    60,     8,
      -1,    13,    78,    68,     8,    -1,    13,    78,    68,    15,
      60,     8,    -1,     9,    69,    60,     8,    -1,    10,    31,
      32,    76,    60,     8,    -1,    18,    -1,    11,    -1,    12,
      -1,    16,    31,    48,    70,    49,    60,    64,    -1,    16,
      31,    32,    31,    48,    70,    49,    60,    64,    -1,    16,
      31,    50,    51,    32,    31,    48,    70,    49,    60,    64,
      -1,    16,    52,    74,    53,    32,    31,    48,    70,    49,
      60,    64,    -1,    -1,    67,     7,    69,    60,    -1,    -1,
       4,    -1,    68,    14,    78,     4,    60,    -1,    68,    14,
      50,    72,    51,    60,    -1,    78,    33,    78,    -1,    78,
      34,    78,    -1,    78,    23,    78,    -1,    78,    24,    78,
      -1,    78,    25,    78,    -1,    78,    26,    78,    -1,    48,
      69,    49,    -1,    54,    69,    -1,    69,    27,    69,    -1,
      69,    28,    69,    -1,    -1,    31,    -1,    31,    50,    51,
      -1,    70,    46,    31,    -1,    70,    46,    31,    50,    51,
      -1,    76,    -1,    79,    -1,    71,    -1,    72,    46,    71,
      -1,    -1,    74,    -1,    73,    47,    74,    -1,    76,    -1,
      74,    76,    -1,    74,    46,    76,    -1,    78,    35,    78,
      -1,    78,    35,    78,    35,    78,    -1,    78,    -1,    75,
      -1,    31,    32,    76,    -1,    31,    50,    78,    51,    32,
      76,    -1,    31,    48,    72,    49,    32,    76,    -1,    31,
      50,    78,    51,    48,    78,    49,    32,    76,    -1,    31,
      32,    77,    -1,    31,    50,    78,    51,    32,    77,    -1,
      31,    48,    72,    49,    32,    77,    -1,    31,    50,    78,
      51,    48,    78,    49,    32,    77,    -1,    31,    32,    79,
      -1,    31,    50,    51,    32,    31,    48,    72,    49,    -1,
      52,    74,    53,    32,    31,    48,    72,    49,    -1,    29,
      -1,    30,    -1,    31,    -1,    31,    50,    78,    51,    -1,
      31,    50,    78,    51,    48,    78,    49,    -1,    31,    50,
      51,    -1,    31,    48,    49,    -1,    31,    48,    72,    49,
      -1,    55,    31,    48,    49,    -1,    55,    31,    48,    72,
      49,    -1,    31,    48,    78,    54,    78,    49,    -1,    36,
      78,    -1,    37,    78,    -1,    48,    76,    49,    -1,    19,
      48,    31,    32,    76,    46,    78,    49,    -1,    78,    37,
      78,    -1,    78,    36,    78,    -1,    78,    40,    78,    -1,
      78,    41,    78,    -1,    78,    42,    78,    -1,    78,    38,
      78,    -1,    78,    39,    78,    -1,    78,    39,    78,    56,
      78,    -1,    78,    20,    78,    -1,    78,    21,    78,    -1,
      78,    22,    78,    -1,    52,    73,    53,    -1,    52,    74,
      53,    -1,    50,    72,    51,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   108,   108,   109,   113,   115,   133,   140,   161,   163,
     168,   172,   174,   178,   178,   178,   178,   181,   181,   181,
     184,   186,   188,   190,   199,   212,   220,   229,   235,   243,
     247,   249,   253,   260,   269,   278,   289,   290,   303,   304,
     306,   316,   328,   330,   332,   334,   336,   338,   340,   346,
     351,   353,   358,   361,   367,   373,   379,   387,   389,   393,
     398,   406,   409,   414,   421,   426,   431,   438,   442,   449,
     451,   455,   461,   468,   475,   483,   489,   496,   503,   511,
     518,   526,   535,   540,   545,   550,   556,   563,   568,   573,
     581,   589,   600,   608,   613,   619,   625,   633,   635,   637,
     639,   641,   643,   645,   647,   655,   657,   659,   661,   667,
     675
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of text\"", "error", "$undefined", "T_UNKNOWN",
  "\"end of line\"", "\"if\"", "\"else\"", "\"elseif\"", "\"end\"",
  "\"while\"", "\"for\"", "\"break\"", "\"continue\"", "\"switch\"",
  "\"case\"", "\"otherwise\"", "\"function\"", "\"endfunction\"",
  "\"return\"", "\"sigma\"", "\">>\"", "\"<<\"", "\"++\"", "\"==\"",
  "\"!=\"", "\"<=\"", "\">=\"", "\"&&\"", "\"||\"", "\"number\"",
  "\"string\"", "\"identifier\"", "'='", "'<'", "'>'", "':'", "'-'", "'+'",
  "'%'", "'@'", "'*'", "'/'", "'^'", "T_NEGATIVE", "T_POSITIVE",
  "T_LOGIC_NOT", "','", "';'", "'('", "')'", "'{'", "'}'", "'['", "']'",
  "'~'", "'#'", "'|'", "$accept", "input", "block_func", "block", "line",
  "line_func", "eol", "func_end", "stmt", "funcdef", "elseif_list",
  "case_list", "condition", "id_list", "arg", "arg_list", "matrix",
  "vector", "range", "exp_range", "assign", "exp", "initcell", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,    61,    60,    62,    58,    45,    43,    37,    64,
      42,    47,    94,   287,   288,   289,    44,    59,    40,    41,
     123,   125,    91,    93,   126,    35,   124
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    57,    58,    58,    59,    59,    60,    60,    61,    61,
      61,    62,    62,    63,    63,    63,    63,    64,    64,    64,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    66,    66,    66,    66,    67,    67,    68,    68,
      68,    68,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,    70,    70,    70,    70,    70,    71,    71,    72,
      72,    73,    73,    73,    74,    74,    74,    75,    75,    76,
      76,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     2,     1,     2,
       2,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       1,     1,     1,     5,     7,     4,     6,     4,     6,     1,
       1,     1,     7,     9,    11,    11,     0,     4,     0,     1,
       5,     6,     3,     3,     3,     3,     3,     3,     3,     2,
       3,     3,     0,     1,     3,     3,     5,     1,     1,     1,
       3,     0,     1,     3,     1,     2,     3,     3,     5,     1,
       1,     3,     6,     6,     9,     3,     6,     6,     9,     3,
       8,     8,     1,     1,     1,     4,     7,     3,     3,     4,
       4,     5,     6,     2,     2,     3,     8,     3,     3,     3,
       3,     3,     3,     3,     5,     3,     3,     3,     3,     3,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     8,     0,     0,     0,    30,    31,     0,     0,
      29,     0,    82,    83,    84,     0,     0,     0,    61,     0,
       0,     0,     0,    11,     4,     0,    12,    22,    21,    20,
       9,    84,    61,     0,     0,     0,     0,     0,    38,     0,
       0,     0,     0,     0,     0,    93,    94,     0,    70,     0,
      69,     0,    62,    64,    69,    49,     0,     1,     5,    16,
      15,    13,    14,    10,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    62,     0,     6,     0,     0,    39,
       0,     0,    52,     0,     0,     0,     0,    71,    75,    79,
      88,    59,     0,    57,    69,    58,    87,     0,    48,    95,
       0,     0,   108,     0,   109,    65,     0,    50,    51,   105,
     106,   107,    44,    45,    46,    47,    42,    43,    98,    97,
     102,   103,    99,   100,   101,     0,    87,     0,   109,     7,
       0,    27,     0,    25,     0,     0,     0,    53,     0,     0,
       0,     0,     0,     0,    89,     0,     0,    85,    67,    63,
      66,     0,    90,     0,     0,    89,    85,     0,     0,    23,
       0,     0,     0,     0,    52,     0,     0,     0,     0,     0,
       0,   110,    60,     0,     0,     0,     0,     0,     0,     0,
      91,   104,     0,     0,     0,    28,     0,     0,    26,     0,
      54,    55,     0,     0,     0,     0,    73,    77,    92,     0,
      72,    76,     0,    68,     0,     0,    24,     0,     0,     0,
       0,     0,    19,    18,    32,    52,    52,     0,     0,    86,
       0,    86,     0,     0,    56,     0,     0,    96,    80,     0,
      81,    33,     0,     0,    74,    78,     0,     0,    34,    35
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    22,    85,    86,    24,    63,   224,    25,    26,
     140,    90,    27,   148,   101,   102,    51,    52,    48,   103,
      28,    29,   105
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -200
static const yytype_int16 yypact[] =
{
     340,    19,  -200,   100,   100,    28,  -200,  -200,  1168,   -23,
    -200,   -20,  -200,  -200,   -11,  1168,  1168,   100,  1168,   100,
      53,    93,   396,  -200,  -200,     7,  -200,    89,  -200,  1266,
    -200,   124,  1168,   656,  1266,   656,    67,  1168,   234,     1,
    1168,    81,  1034,   925,  1061,  -200,  -200,   -18,  -200,    46,
    1243,     8,   953,  -200,   607,  -200,    55,  -200,  -200,  -200,
    -200,  -200,  -200,  -200,   100,   100,  1168,  1168,  1168,  1168,
    1168,  1168,  1168,  1168,  1168,  1168,  1168,  1168,  1168,  1168,
    1168,  1168,   925,  1074,   966,   448,  -200,   708,  1168,  -200,
      11,    91,    95,    77,   994,   107,  1101,  -200,  -200,  -200,
    -200,  -200,   -22,  -200,   326,  -200,   114,  1197,  -200,  -200,
    1168,  1168,  -200,  1168,   128,  -200,  1007,  -200,   141,   123,
     123,   223,   710,   710,   710,   710,   710,   710,   223,   223,
     123,    48,   133,   133,   133,    -3,  -200,  1220,  -200,  -200,
     126,  -200,   916,  -200,  1128,   916,   130,   129,    64,   144,
     148,  1168,    45,  1101,   150,  1168,   152,   -12,   659,  1155,
    -200,   156,  -200,    72,  1168,  -200,   140,   916,   100,  -200,
     760,  1101,   287,   812,    95,   138,   160,   916,   161,   164,
     151,  -200,  -200,  1195,   382,   153,  1195,  1168,  1168,   158,
    -200,   710,  1168,   864,   656,  -200,    51,   916,  -200,    78,
    -200,   149,   284,   162,   163,  1168,  -200,  -200,  -200,  1101,
    -200,  -200,   450,   710,  1101,   502,  -200,   500,   916,   552,
     916,   147,  -200,  -200,  -200,    95,    95,   554,    92,   172,
      94,  -200,   604,   284,  -200,   110,   112,  -200,  -200,  1195,
    -200,  -200,   916,   916,  -200,  -200,   284,   284,  -200,  -200
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -200,  -200,  -200,    63,     0,   191,  -200,  -199,  -200,  -200,
    -200,  -200,    41,  -169,    61,   -64,  -200,   -28,  -200,    83,
     -39,    -2,   173
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -42
static const yytype_int16 yytable[] =
{
      23,    34,    34,    98,    84,   199,    38,    59,    39,    64,
      65,    60,    94,    45,    46,    50,    54,    34,   135,   143,
     186,    42,    23,    30,   153,   144,   145,   154,    41,    40,
      54,   108,   152,    91,   241,    54,   187,    43,    54,    44,
      54,   104,   107,   153,    33,    35,   165,   248,   249,    92,
      54,    93,   163,    61,    62,   111,   235,   236,    47,    36,
      55,   112,    34,    34,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     104,   137,    54,   159,    56,   139,    54,   139,    79,    80,
      81,   153,    54,    57,    54,   109,   181,   153,    87,    88,
      49,    53,   218,   116,   164,   117,   118,   196,   158,    54,
     176,    54,    95,   177,    54,    53,    64,    65,   153,    11,
      49,   190,   146,    53,   176,    97,   147,   220,   149,    12,
      13,    31,   167,   168,   169,   115,    15,    16,   153,   151,
     153,   238,   172,   240,   207,   228,   156,   211,    17,    54,
     230,    54,    32,   184,    19,    20,   176,    54,   176,   242,
     161,   243,   191,    79,    80,    81,    34,   115,    64,    54,
     139,   142,    82,   139,    83,    81,   178,   115,   174,   175,
     179,    54,   183,   185,    54,   212,   213,   189,   192,   200,
     215,   201,   203,   139,    53,   204,   160,   205,   234,   221,
     245,   209,   139,   227,   239,   170,   214,    54,   173,   194,
     225,   226,    54,    58,   182,    99,     0,   139,     0,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     193,     0,   139,   139,   180,     0,     0,    54,    89,     0,
     202,     0,   115,    66,    67,     0,   139,   139,     0,     0,
       0,     0,     0,     0,    66,    67,    68,   217,     0,     0,
     219,    77,    78,    79,    80,    81,   206,     0,     0,   210,
      75,    76,    77,    78,    79,    80,    81,     0,     0,     0,
       0,   232,     0,   233,   222,     1,     0,     0,     2,     3,
       0,   197,     0,     4,     5,     6,     7,     8,     0,     0,
     -17,   223,    10,    11,     0,   246,   247,    66,    67,    68,
       0,     0,     0,    12,    13,    14,     0,     0,     0,     0,
      15,    16,   244,    75,    76,    77,    78,    79,    80,    81,
       0,     0,    17,     0,     0,     0,    18,     0,    19,    20,
      -2,     1,     0,     0,     2,     3,    66,    67,    68,     4,
       5,     6,     7,     8,     0,     0,     9,     0,    10,    11,
       0,   110,    75,    76,    77,    78,    79,    80,    81,    12,
      13,    14,     0,     0,     0,     0,    15,    16,     0,     0,
     155,     0,     0,     0,     0,     0,     0,     0,    17,     0,
       0,     0,    18,     0,    19,    20,    -3,     1,     0,     0,
       2,     3,    66,    67,    68,     4,     5,     6,     7,     8,
       0,     0,     9,     0,    10,    11,     0,     0,    75,    76,
      77,    78,    79,    80,    81,    12,    13,    14,     0,     0,
       0,   208,    15,    16,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,     0,     0,     0,    18,     1,
      19,    20,     2,     3,   -36,   -36,   -36,     4,     5,     6,
       7,     8,     0,     0,     0,     0,    10,    11,     0,     0,
      66,    67,    68,     0,     0,     0,     0,    12,    13,    14,
       0,     0,     0,     0,    15,    16,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,    17,     0,     0,   229,
      18,     1,    19,    20,     2,     3,   -37,   -37,   -37,     4,
       5,     6,     7,     8,     0,     0,     0,     0,    10,    11,
       0,     0,    66,    67,    68,     0,     0,     0,     0,    12,
      13,    14,     0,     0,     0,     0,    15,    16,    75,    76,
      77,    78,    79,    80,    81,     0,     0,     0,    17,     0,
       0,   231,    18,     1,    19,    20,     2,     3,     0,     0,
     -40,     4,     5,     6,     7,     8,   -40,   -40,     0,     0,
      10,    11,     0,     0,    66,    67,    68,     0,     0,     0,
       0,    12,    13,    14,     0,     0,     0,     0,    15,    16,
      75,    76,    77,    78,    79,    80,    81,     0,     0,     0,
      17,     0,     0,   237,    18,     1,    19,    20,     2,     3,
       0,     0,   -41,     4,     5,     6,     7,     8,   -41,   -41,
       0,     0,    10,    11,     0,     0,     0,    66,    67,    68,
       0,     0,     0,    12,    13,    14,     0,     0,     0,     0,
      15,    16,   110,    75,    76,    77,    78,    79,    80,    81,
       0,     0,    17,     0,     0,     0,    18,     1,    19,    20,
       2,     3,     0,     0,     0,     4,     5,     6,     7,     8,
       0,     0,     0,     0,    10,    11,     0,     0,     0,    66,
      67,    68,     0,    64,    65,    12,    13,    14,     0,     0,
       0,     0,    15,    16,   188,    75,    76,    77,    78,    79,
      80,    81,     0,     0,    17,     0,     0,     0,    18,     1,
      19,    20,     2,     3,     0,     0,   141,     4,     5,     6,
       7,     8,     0,     0,     0,     0,    10,    11,     0,     0,
      66,    67,    68,     0,     0,     0,     0,    12,    13,    14,
       0,     0,     0,     0,    15,    16,    75,    76,    77,    78,
      79,    80,    81,     0,     0,     0,    17,     0,     0,     0,
      18,     1,    19,    20,     2,     3,     0,     0,   195,     4,
       5,     6,     7,     8,     0,     0,     0,     0,    10,    11,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    12,
      13,    14,     0,     0,     0,     0,    15,    16,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,     0,
       0,     0,    18,     1,    19,    20,     2,     3,     0,     0,
     198,     4,     5,     6,     7,     8,     0,     0,     0,     0,
      10,    11,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,    13,    14,     0,     0,     0,     0,    15,    16,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      17,     0,     0,     0,    18,     1,    19,    20,     2,     3,
       0,     0,   216,     4,     5,     6,     7,     8,     0,     0,
       0,     0,    10,    11,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,    13,    14,     0,     0,     0,     0,
      15,    16,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,     0,     0,     0,    18,     1,    19,    20,
       2,     3,     0,     0,     0,     4,     5,     6,     7,     8,
       0,     0,     0,     0,    10,    11,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,    13,    14,     0,     0,
       0,     0,    15,    16,    12,    13,    31,     0,     0,     0,
       0,    15,    16,     0,    17,     0,     0,     0,    18,     0,
      19,    20,    11,    37,   100,    96,     0,    32,     0,     0,
      20,     0,    12,    13,    31,    11,     0,     0,     0,    15,
      16,     0,     0,     0,     0,    12,    13,    31,     0,   113,
       0,    37,    15,    16,     0,    32,   114,     0,    20,     0,
       0,     0,   113,    11,    37,     0,     0,     0,    32,   138,
       0,    20,     0,    12,    13,    31,    11,     0,     0,     0,
      15,    16,     0,     0,     0,     0,    12,    13,    31,     0,
     113,     0,    37,    15,    16,     0,    32,   150,     0,    20,
       0,     0,     0,    11,     0,    37,   162,    96,     0,    32,
       0,     0,    20,    12,    13,    14,     0,     0,     0,     0,
      15,    16,     0,     0,     0,     0,     0,     0,     0,     0,
      11,     0,    37,     0,    96,     0,    18,     0,     0,    20,
      12,    13,    31,    11,     0,     0,     0,    15,    16,     0,
       0,     0,     0,    12,    13,    31,     0,     0,     0,    37,
      15,    16,   106,    32,     0,     0,    20,     0,     0,     0,
      11,     0,    37,     0,     0,   136,    32,     0,     0,    20,
      12,    13,    31,     0,     0,     0,     0,    15,    16,     0,
       0,     0,     0,     0,     0,     0,     0,    11,     0,    37,
       0,    96,     0,    32,     0,     0,    20,    12,    13,    31,
       0,     0,     0,     0,    15,    16,     0,     0,     0,     0,
       0,     0,     0,     0,    11,     0,    37,     0,   171,     0,
      32,     0,     0,    20,    12,    13,    31,    11,     0,     0,
       0,    15,    16,     0,     0,     0,     0,    12,    13,    31,
       0,   113,     0,    37,    15,    16,     0,    32,     0,     0,
      20,     0,     0,     0,    11,     0,    37,    66,    67,    68,
      32,     0,     0,    20,    12,    13,    14,     0,     0,     0,
       0,    15,    16,    75,    76,    77,    78,    79,    80,    81,
      66,    67,    68,    37,     0,     0,     0,    18,   157,     0,
      20,     0,     0,     0,     0,     0,    75,    76,    77,    78,
      79,    80,    81,    66,    67,    68,    69,    70,    71,    72,
       0,   166,     0,     0,     0,     0,    73,    74,   110,    75,
      76,    77,    78,    79,    80,    81,    66,    67,    68,    69,
      70,    71,    72,     0,     0,     0,     0,     0,     0,    73,
      74,     0,    75,    76,    77,    78,    79,    80,    81
};

static const yytype_int16 yycheck[] =
{
       0,     3,     4,    42,    32,   174,     8,     0,    31,    27,
      28,     4,    40,    15,    16,    17,    18,    19,    82,     8,
      32,    32,    22,     4,    46,    14,    15,    49,    48,    52,
      32,    49,    96,    32,   233,    37,    48,    48,    40,    50,
      42,    43,    44,    46,     3,     4,    49,   246,   247,    48,
      52,    50,   116,    46,    47,    47,   225,   226,    17,    31,
      19,    53,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,   111,    31,    85,    88,    87,    40,    41,
      42,    46,    94,     0,    96,    49,    51,    46,    35,    32,
      17,    18,    51,    48,    56,    64,    65,   171,   110,   111,
      46,   113,    31,    49,   116,    32,    27,    28,    46,    19,
      37,    49,    31,    40,    46,    42,    31,    49,    51,    29,
      30,    31,     6,     7,     8,    52,    36,    37,    46,    32,
      46,    49,   144,    49,   183,   209,    32,   186,    48,   151,
     214,   153,    52,   155,    54,    55,    46,   159,    46,    49,
      32,    49,   164,    40,    41,    42,   168,    84,    27,   171,
     170,    88,    48,   173,    50,    42,    32,    94,    48,    50,
      32,   183,    32,    31,   186,   187,   188,    31,    48,    51,
     192,    31,    31,   193,   111,    31,   113,    46,    51,    50,
     239,    48,   202,   205,    32,   142,    48,   209,   145,   168,
      48,    48,   214,    22,   153,    42,    -1,   217,    -1,   219,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     167,    -1,   232,   233,   151,    -1,    -1,   239,     4,    -1,
     177,    -1,   159,    20,    21,    -1,   246,   247,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    21,    22,   194,    -1,    -1,
     197,    38,    39,    40,    41,    42,   183,    -1,    -1,   186,
      36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
      -1,   218,    -1,   220,     0,     1,    -1,    -1,     4,     5,
      -1,     4,    -1,     9,    10,    11,    12,    13,    -1,    -1,
      16,    17,    18,    19,    -1,   242,   243,    20,    21,    22,
      -1,    -1,    -1,    29,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,   239,    36,    37,    38,    39,    40,    41,    42,
      -1,    -1,    48,    -1,    -1,    -1,    52,    -1,    54,    55,
       0,     1,    -1,    -1,     4,     5,    20,    21,    22,     9,
      10,    11,    12,    13,    -1,    -1,    16,    -1,    18,    19,
      -1,    35,    36,    37,    38,    39,    40,    41,    42,    29,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    -1,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    52,    -1,    54,    55,     0,     1,    -1,    -1,
       4,     5,    20,    21,    22,     9,    10,    11,    12,    13,
      -1,    -1,    16,    -1,    18,    19,    -1,    -1,    36,    37,
      38,    39,    40,    41,    42,    29,    30,    31,    -1,    -1,
      -1,    49,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    52,     1,
      54,    55,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,
      20,    21,    22,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    48,    -1,    -1,    49,
      52,     1,    54,    55,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,
      -1,    -1,    20,    21,    22,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    36,    37,
      38,    39,    40,    41,    42,    -1,    -1,    -1,    48,    -1,
      -1,    49,    52,     1,    54,    55,     4,     5,    -1,    -1,
       8,     9,    10,    11,    12,    13,    14,    15,    -1,    -1,
      18,    19,    -1,    -1,    20,    21,    22,    -1,    -1,    -1,
      -1,    29,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      36,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
      48,    -1,    -1,    49,    52,     1,    54,    55,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    14,    15,
      -1,    -1,    18,    19,    -1,    -1,    -1,    20,    21,    22,
      -1,    -1,    -1,    29,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    35,    36,    37,    38,    39,    40,    41,    42,
      -1,    -1,    48,    -1,    -1,    -1,    52,     1,    54,    55,
       4,     5,    -1,    -1,    -1,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    20,
      21,    22,    -1,    27,    28,    29,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    35,    36,    37,    38,    39,    40,
      41,    42,    -1,    -1,    48,    -1,    -1,    -1,    52,     1,
      54,    55,     4,     5,    -1,    -1,     8,     9,    10,    11,
      12,    13,    -1,    -1,    -1,    -1,    18,    19,    -1,    -1,
      20,    21,    22,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      52,     1,    54,    55,     4,     5,    -1,    -1,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    18,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    -1,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
      -1,    -1,    52,     1,    54,    55,     4,     5,    -1,    -1,
       8,     9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,
      18,    19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    29,    30,    31,    -1,    -1,    -1,    -1,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    -1,    -1,    -1,    52,     1,    54,    55,     4,     5,
      -1,    -1,     8,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    52,     1,    54,    55,
       4,     5,    -1,    -1,    -1,     9,    10,    11,    12,    13,
      -1,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    29,    30,    31,    -1,    -1,
      -1,    -1,    36,    37,    29,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    -1,    48,    -1,    -1,    -1,    52,    -1,
      54,    55,    19,    48,    49,    50,    -1,    52,    -1,    -1,
      55,    -1,    29,    30,    31,    19,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    46,
      -1,    48,    36,    37,    -1,    52,    53,    -1,    55,    -1,
      -1,    -1,    46,    19,    48,    -1,    -1,    -1,    52,    53,
      -1,    55,    -1,    29,    30,    31,    19,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,
      46,    -1,    48,    36,    37,    -1,    52,    53,    -1,    55,
      -1,    -1,    -1,    19,    -1,    48,    49,    50,    -1,    52,
      -1,    -1,    55,    29,    30,    31,    -1,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    48,    -1,    50,    -1,    52,    -1,    -1,    55,
      29,    30,    31,    19,    -1,    -1,    -1,    36,    37,    -1,
      -1,    -1,    -1,    29,    30,    31,    -1,    -1,    -1,    48,
      36,    37,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,
      19,    -1,    48,    -1,    -1,    51,    52,    -1,    -1,    55,
      29,    30,    31,    -1,    -1,    -1,    -1,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    -1,    48,
      -1,    50,    -1,    52,    -1,    -1,    55,    29,    30,    31,
      -1,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    48,    -1,    50,    -1,
      52,    -1,    -1,    55,    29,    30,    31,    19,    -1,    -1,
      -1,    36,    37,    -1,    -1,    -1,    -1,    29,    30,    31,
      -1,    46,    -1,    48,    36,    37,    -1,    52,    -1,    -1,
      55,    -1,    -1,    -1,    19,    -1,    48,    20,    21,    22,
      52,    -1,    -1,    55,    29,    30,    31,    -1,    -1,    -1,
      -1,    36,    37,    36,    37,    38,    39,    40,    41,    42,
      20,    21,    22,    48,    -1,    -1,    -1,    52,    51,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    36,    37,    38,    39,
      40,    41,    42,    20,    21,    22,    23,    24,    25,    26,
      -1,    51,    -1,    -1,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      34,    -1,    36,    37,    38,    39,    40,    41,    42
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     4,     5,     9,    10,    11,    12,    13,    16,
      18,    19,    29,    30,    31,    36,    37,    48,    52,    54,
      55,    58,    59,    61,    62,    65,    66,    69,    77,    78,
       4,    31,    52,    69,    78,    69,    31,    48,    78,    31,
      52,    48,    32,    48,    50,    78,    78,    69,    75,    76,
      78,    73,    74,    76,    78,    69,    31,     0,    62,     0,
       4,    46,    47,    63,    27,    28,    20,    21,    22,    23,
      24,    25,    26,    33,    34,    36,    37,    38,    39,    40,
      41,    42,    48,    50,    74,    60,    61,    60,    32,     4,
      68,    32,    48,    50,    74,    31,    50,    76,    77,    79,
      49,    71,    72,    76,    78,    79,    51,    78,    49,    49,
      35,    47,    53,    46,    53,    76,    48,    69,    69,    78,
      78,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      78,    78,    78,    78,    78,    72,    51,    78,    53,    61,
      67,     8,    76,     8,    14,    15,    31,    31,    70,    51,
      53,    32,    72,    46,    49,    54,    32,    51,    78,    74,
      76,    32,    49,    72,    56,    49,    51,     6,     7,     8,
      60,    50,    78,    60,    48,    50,    46,    49,    32,    32,
      76,    51,    71,    32,    78,    31,    32,    48,    35,    31,
      49,    78,    48,    60,    69,     8,    72,     4,     8,    70,
      51,    31,    60,    31,    31,    46,    76,    77,    49,    48,
      76,    77,    78,    78,    48,    78,     8,    60,    51,    60,
      49,    50,     0,    17,    64,    48,    48,    78,    72,    49,
      72,    49,    60,    60,    51,    70,    70,    49,    49,    32,
      49,    64,    49,    49,    76,    77,    60,    60,    64,    64
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (pproot, errmsg, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, pproot, errmsg); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pproot, errmsg);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, AstNode **pproot, char **errmsg)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, pproot, errmsg)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    AstNode **pproot;
    char **errmsg;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , pproot, errmsg);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, pproot, errmsg); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, AstNode **pproot, char **errmsg)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, pproot, errmsg)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    AstNode **pproot;
    char **errmsg;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pproot);
  YYUSE (errmsg);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {
      case 30: /* "\"string\"" */

/* Line 1000 of yacc.c  */
#line 92 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1552 "psycon.tab.c"
	break;
      case 31: /* "\"identifier\"" */

/* Line 1000 of yacc.c  */
#line 92 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding string \"%s\"\n", (yyvaluep->str));
#endif
  free((yyvaluep->str));
};

/* Line 1000 of yacc.c  */
#line 1566 "psycon.tab.c"
	break;
      case 59: /* "block_func" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1580 "psycon.tab.c"
	break;
      case 60: /* "block" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1594 "psycon.tab.c"
	break;
      case 61: /* "line" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1608 "psycon.tab.c"
	break;
      case 62: /* "line_func" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1622 "psycon.tab.c"
	break;
      case 65: /* "stmt" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1636 "psycon.tab.c"
	break;
      case 66: /* "funcdef" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1650 "psycon.tab.c"
	break;
      case 67: /* "elseif_list" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1664 "psycon.tab.c"
	break;
      case 68: /* "case_list" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1678 "psycon.tab.c"
	break;
      case 69: /* "condition" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1692 "psycon.tab.c"
	break;
      case 70: /* "id_list" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1706 "psycon.tab.c"
	break;
      case 71: /* "arg" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1720 "psycon.tab.c"
	break;
      case 72: /* "arg_list" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1734 "psycon.tab.c"
	break;
      case 73: /* "matrix" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1748 "psycon.tab.c"
	break;
      case 74: /* "vector" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1762 "psycon.tab.c"
	break;
      case 75: /* "range" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1776 "psycon.tab.c"
	break;
      case 76: /* "exp_range" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1790 "psycon.tab.c"
	break;
      case 77: /* "assign" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1804 "psycon.tab.c"
	break;
      case 78: /* "exp" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1818 "psycon.tab.c"
	break;
      case 79: /* "initcell" */

/* Line 1000 of yacc.c  */
#line 85 "psycon.y"
	{
#ifdef DEBUG
    printf("discarding node %s\n", getAstNodeName((yyvaluep->pnode)));
#endif
  yydeleteAstNode((yyvaluep->pnode), 0);
};

/* Line 1000 of yacc.c  */
#line 1832 "psycon.tab.c"
	break;

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (AstNode **pproot, char **errmsg);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (AstNode **pproot, char **errmsg)
#else
int
yyparse (pproot, errmsg)
    AstNode **pproot;
    char **errmsg;
#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

/* User initialization code.  */

/* Line 1242 of yacc.c  */
#line 76 "psycon.y"
{
  if (ErrorMsg) {
	free(ErrorMsg);
	ErrorMsg = NULL;
  }
  *errmsg = NULL;
}

/* Line 1242 of yacc.c  */
#line 1993 "psycon.tab.c"

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 108 "psycon.y"
    { *pproot = NULL;;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 110 "psycon.y"
    { *pproot = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 114 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 116 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode) == NULL)
				(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
			else if ((yyvsp[(1) - (2)].pnode)->type == NODE_BLOCK) {
				(yyvsp[(1) - (2)].pnode)->LastChild = (yyvsp[(1) - (2)].pnode)->LastChild->next = (yyvsp[(2) - (2)].pnode);
				(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
			} else {
				(yyval.pnode) = newAstNode(NODE_BLOCK, (yyloc));
				(yyval.pnode)->child = (yyvsp[(1) - (2)].pnode);
				(yyval.pnode)->LastChild = (yyval.pnode)->child->next = (yyvsp[(2) - (2)].pnode);
			}
		} else
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 134 "psycon.y"
    {
		if ((yyvsp[(1) - (1)].pnode))
			(yyval.pnode) = (yyvsp[(1) - (1)].pnode);
		else
			(yyval.pnode) = newAstNode(NODE_BLOCK, (yyloc));
	;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 141 "psycon.y"
    {
		if ((yyvsp[(2) - (2)].pnode)) {
			if ((yyvsp[(1) - (2)].pnode)->type == NODE_BLOCK) {
				if ((yyvsp[(1) - (2)].pnode)->LastChild) {
					(yyvsp[(1) - (2)].pnode)->LastChild = (yyvsp[(1) - (2)].pnode)->LastChild->next = (yyvsp[(2) - (2)].pnode);
					(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
				} else {
					(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
					free((yyvsp[(1) - (2)].pnode));
				}
			} else {
				(yyval.pnode) = newAstNode(NODE_BLOCK, (yyloc));
				(yyval.pnode)->child = (yyvsp[(1) - (2)].pnode);
				(yyval.pnode)->LastChild = (yyval.pnode)->child->next = (yyvsp[(2) - (2)].pnode);
			}
		} else
			(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 162 "psycon.y"
    { (yyval.pnode) = NULL;;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 164 "psycon.y"
    {
		(yyval.pnode) = NULL;
		yyerrok;
	;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 169 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (2)].pnode);;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 173 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 175 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 185 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 187 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 189 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 191 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(4) - (5)].pnode);
		(yyvsp[(2) - (5)].pnode)->next = (yyvsp[(3) - (5)].pnode);
		(yyvsp[(3) - (5)].pnode)->next = (yyvsp[(4) - (5)].pnode)->child;
		(yyval.pnode)->child = (yyvsp[(2) - (5)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 200 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(4) - (7)].pnode);
		(yyvsp[(2) - (7)].pnode)->next = (yyvsp[(3) - (7)].pnode);
		if ((yyvsp[(4) - (7)].pnode)->LastChild) {
			(yyvsp[(3) - (7)].pnode)->next = (yyvsp[(4) - (7)].pnode)->child;
			(yyvsp[(4) - (7)].pnode)->LastChild->next = (yyvsp[(6) - (7)].pnode);
		} else
			(yyvsp[(3) - (7)].pnode)->next = (yyvsp[(6) - (7)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (7)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 213 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (4)].pnode);
		(yyvsp[(2) - (4)].pnode)->next = (yyvsp[(3) - (4)].pnode)->child;
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 221 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (6)].pnode);
		(yyvsp[(2) - (6)].pnode)->next = (yyvsp[(3) - (6)].pnode)->child;
		(yyvsp[(3) - (6)].pnode)->LastChild->next = (yyvsp[(5) - (6)].pnode);
		(yyval.pnode)->child = (yyvsp[(2) - (6)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 230 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_WHILE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (4)].pnode);
		(yyvsp[(2) - (4)].pnode)->next = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 236 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FOR, (yyloc));
		(yyval.pnode)->child = newAstNode('=', (yylsp[(2) - (6)]));
		(yyval.pnode)->child->str = (yyvsp[(2) - (6)].str);
		(yyval.pnode)->child->child = (yyvsp[(4) - (6)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 244 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_RETURN, (yyloc));
	;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 248 "psycon.y"
    { (yyval.pnode) = newAstNode(T_BREAK, (yyloc));;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 250 "psycon.y"
    { (yyval.pnode) = newAstNode(T_CONTINUE, (yyloc));;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 254 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->str = (yyvsp[(2) - (7)].str);
		(yyval.pnode)->child = (yyvsp[(4) - (7)].pnode);
		(yyvsp[(4) - (7)].pnode)->next = (yyvsp[(6) - (7)].pnode);
	;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 261 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->str = (yyvsp[(4) - (9)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (9)].pnode);
		(yyvsp[(6) - (9)].pnode)->next = (yyvsp[(8) - (9)].pnode);
		(yyvsp[(8) - (9)].pnode)->next = newAstNode(T_ID, (yylsp[(2) - (9)]));
		(yyvsp[(8) - (9)].pnode)->next->str = (yyvsp[(2) - (9)].str);
	;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 270 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->str = (yyvsp[(6) - (11)].str);
		(yyval.pnode)->child = (yyvsp[(8) - (11)].pnode);
		(yyvsp[(8) - (11)].pnode)->next = (yyvsp[(10) - (11)].pnode);
		(yyvsp[(10) - (11)].pnode)->next = newAstNode(NODE_INITCELL, (yylsp[(2) - (11)]));
		(yyvsp[(10) - (11)].pnode)->next->str = (yyvsp[(2) - (11)].str);
	;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 279 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_FUNCTION, (yyloc));
		(yyval.pnode)->str = (yyvsp[(6) - (11)].str);
		(yyval.pnode)->child = (yyvsp[(8) - (11)].pnode);
		(yyvsp[(8) - (11)].pnode)->next = (yyvsp[(10) - (11)].pnode);
		(yyvsp[(10) - (11)].pnode)->next = (yyvsp[(3) - (11)].pnode);
	;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 289 "psycon.y"
    { (yyval.pnode) = newAstNode(T_IF, (yyloc));;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 291 "psycon.y"
    {
		if ((yyvsp[(1) - (4)].pnode)->child)
			(yyvsp[(1) - (4)].pnode)->LastChild->next = (yyvsp[(3) - (4)].pnode);
		else
			(yyvsp[(1) - (4)].pnode)->child = (yyvsp[(3) - (4)].pnode);
		(yyvsp[(3) - (4)].pnode)->next = (yyvsp[(4) - (4)].pnode);
		(yyvsp[(1) - (4)].pnode)->LastChild = (yyvsp[(4) - (4)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (4)].pnode);
	;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 303 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 305 "psycon.y"
    { (yyval.pnode) = newAstNode(T_SWITCH, (yyloc));;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 307 "psycon.y"
    {
		if ((yyvsp[(1) - (5)].pnode)->child)
			(yyvsp[(1) - (5)].pnode)->LastChild->next = (yyvsp[(3) - (5)].pnode);
		else
			(yyvsp[(1) - (5)].pnode)->child = (yyvsp[(3) - (5)].pnode);
		(yyvsp[(3) - (5)].pnode)->next = (yyvsp[(5) - (5)].pnode);
		(yyvsp[(1) - (5)].pnode)->LastChild = (yyvsp[(5) - (5)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (5)].pnode);
	;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 317 "psycon.y"
    {
		if ((yyvsp[(1) - (6)].pnode)->child)
			(yyvsp[(1) - (6)].pnode)->LastChild->next = (yyvsp[(4) - (6)].pnode);
		else
			(yyvsp[(1) - (6)].pnode)->child = (yyvsp[(4) - (6)].pnode);
		(yyvsp[(4) - (6)].pnode)->next = (yyvsp[(6) - (6)].pnode);
		(yyvsp[(1) - (6)].pnode)->LastChild = (yyvsp[(6) - (6)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (6)].pnode);
	;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 329 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('<', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 331 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('>', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 333 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_COMP_EQ, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 335 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_COMP_NE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 337 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_COMP_LE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 339 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_COMP_GE, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 341 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 347 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_LOGIC_NOT, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 352 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_AND, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 354 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_LOGIC_OR, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 358 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IDLIST, (yyloc));
	;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 362 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IDLIST, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->LastChild = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->LastChild->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 368 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IDLIST, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->LastChild = newAstNode(NODE_INITCELL, (yyloc));
		(yyval.pnode)->LastChild->str = (yyvsp[(1) - (3)].str);
	;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 374 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->LastChild = (yyvsp[(1) - (3)].pnode)->LastChild->next = newAstNode(T_ID, (yylsp[(3) - (3)]));
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
		(yyval.pnode)->LastChild->str = (yyvsp[(3) - (3)].str);
	;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 380 "psycon.y"
    {
		(yyvsp[(1) - (5)].pnode)->LastChild = (yyvsp[(1) - (5)].pnode)->LastChild->next = newAstNode(NODE_INITCELL, (yylsp[(3) - (5)]));
		(yyval.pnode) = (yyvsp[(1) - (5)].pnode);
		(yyval.pnode)->LastChild->str = (yyvsp[(3) - (5)].str);
	;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 388 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 390 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 394 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_ARGS, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->LastChild = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 399 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->LastChild = (yyvsp[(1) - (3)].pnode)->LastChild->next = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 406 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_MATRIX, (yyloc));
	;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 410 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_MATRIX, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->LastChild = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 415 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->LastChild = (yyvsp[(1) - (3)].pnode)->LastChild->next = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 422 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_VECTOR, (yyloc));
		(yyval.pnode)->child = (yyval.pnode)->LastChild = (yyvsp[(1) - (1)].pnode);
	;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 427 "psycon.y"
    {
		(yyvsp[(1) - (2)].pnode)->LastChild = (yyvsp[(1) - (2)].pnode)->LastChild->next = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (2)].pnode);
	;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 432 "psycon.y"
    {
		(yyvsp[(1) - (3)].pnode)->LastChild = (yyvsp[(1) - (3)].pnode)->LastChild->next = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode) = (yyvsp[(1) - (3)].pnode);
	;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 439 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));
	;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 443 "psycon.y"
    {
		(yyval.pnode) = makeFunctionCall(":", (yyvsp[(1) - (5)].pnode), (yyvsp[(5) - (5)].pnode), (yyloc));
		(yyval.pnode)->LastChild = (yyvsp[(5) - (5)].pnode)->next = (yyvsp[(3) - (5)].pnode);
	;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 450 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 452 "psycon.y"
    { (yyval.pnode) = (yyvsp[(1) - (1)].pnode);;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 456 "psycon.y"
    {
		(yyval.pnode) = newAstNode('=', (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (3)].str);
		(yyval.pnode)->child = (yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 462 "psycon.y"
    {
		(yyval.pnode) = newAstNode('=', (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (6)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (6)].pnode);
		(yyvsp[(6) - (6)].pnode)->next = (yyvsp[(3) - (6)].pnode);
	;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 469 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IXASSIGN, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (6)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (6)].pnode);
		(yyvsp[(6) - (6)].pnode)->next = (yyvsp[(3) - (6)].pnode);
	;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 476 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IXASSIGN, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (9)].str);
		(yyval.pnode)->child = (yyvsp[(9) - (9)].pnode);
		(yyvsp[(9) - (9)].pnode)->next = (yyvsp[(6) - (9)].pnode);
		(yyvsp[(6) - (9)].pnode)->next = (yyvsp[(3) - (9)].pnode);
	;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 484 "psycon.y"
    {
		(yyval.pnode) = newAstNode('=', (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (3)].str);
		(yyval.pnode)->child = (yyvsp[(3) - (3)].pnode);
	;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 490 "psycon.y"
    {
		(yyval.pnode) = newAstNode('=', (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (6)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (6)].pnode);
		(yyvsp[(6) - (6)].pnode)->next = (yyvsp[(3) - (6)].pnode);
	;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 497 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IXASSIGN, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (6)].str);
		(yyval.pnode)->child = (yyvsp[(6) - (6)].pnode);
		(yyvsp[(6) - (6)].pnode)->next = (yyvsp[(3) - (6)].pnode);
	;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 504 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_IXASSIGN, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (9)].str);
		(yyval.pnode)->child = (yyvsp[(9) - (9)].pnode);
		(yyvsp[(9) - (9)].pnode)->next = (yyvsp[(6) - (9)].pnode);
		(yyvsp[(6) - (9)].pnode)->next = (yyvsp[(3) - (9)].pnode);
	;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 512 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (3)].pnode);
		(yyval.pnode)->str = (yyvsp[(1) - (3)].str);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 519 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_CALL, (yyloc));
		(yyval.pnode)->str = (yyvsp[(5) - (8)].str);
		(yyval.pnode)->child = (yyvsp[(7) - (8)].pnode);
		(yyvsp[(7) - (8)].pnode)->next = newAstNode(NODE_INITCELL, (yylsp[(1) - (8)]));
		(yyvsp[(7) - (8)].pnode)->next->str = (yyvsp[(1) - (8)].str);
	;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 527 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_CALL, (yyloc));
		(yyval.pnode)->str = (yyvsp[(5) - (8)].str);
		(yyval.pnode)->child = (yyvsp[(7) - (8)].pnode);
		(yyvsp[(7) - (8)].pnode)->next = (yyvsp[(2) - (8)].pnode);
	;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 536 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NUMBER, (yyloc));
		(yyval.pnode)->dval = (yyvsp[(1) - (1)].dval);
	;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 541 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_STRING, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 546 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (1)].str);
	;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 551 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		(yyval.pnode)->child = (yyvsp[(3) - (4)].pnode);
	;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 557 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_ID, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (7)].str);
		(yyval.pnode)->child = (yyvsp[(3) - (7)].pnode);
		(yyvsp[(3) - (7)].pnode)->next = (yyvsp[(6) - (7)].pnode);
	;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 564 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_INITCELL, (yyloc));
		(yyval.pnode)->str = (yyvsp[(1) - (3)].str);
	;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 569 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_CALL, (yyloc));
 		(yyval.pnode)->str = (yyvsp[(1) - (3)].str);
	;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 574 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(3) - (4)].pnode);
		(yyval.pnode)->type = NODE_CALL;
 		(yyval.pnode)->str = (yyvsp[(1) - (4)].str);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 582 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_CALL, (yyloc));
 		(yyval.pnode)->str = malloc(strlen((yyvsp[(2) - (4)].str))+2);
 		(yyval.pnode)->str[0] = '#';
 		strcpy((yyval.pnode)->str+1, (yyvsp[(2) - (4)].str));
 		free((yyvsp[(2) - (4)].str));
	;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 590 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(4) - (5)].pnode);
		(yyval.pnode)->type = NODE_CALL;
 		(yyval.pnode)->str = malloc(strlen((yyvsp[(2) - (5)].str))+2);
 		(yyval.pnode)->str[0] = '#';
 		strcpy((yyval.pnode)->str+1, (yyvsp[(2) - (5)].str));
 		free((yyvsp[(2) - (5)].str));
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 601 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_EXTRACT, (yyloc));
		(yyval.pnode)->child = newAstNode(T_ID, (yylsp[(1) - (6)]));
		(yyval.pnode)->child->str = (yyvsp[(1) - (6)].str);
		(yyval.pnode)->child->next = (yyvsp[(3) - (6)].pnode);
		(yyvsp[(3) - (6)].pnode)->next = (yyvsp[(5) - (6)].pnode);
	;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 609 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_NEGATIVE, (yyloc));
		(yyval.pnode)->child = (yyvsp[(2) - (2)].pnode);
	;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 614 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (2)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 620 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 626 "psycon.y"
    {
		(yyval.pnode) = newAstNode(T_SIGMA, (yyloc));
		(yyval.pnode)->child = newAstNode('=', (yylsp[(3) - (8)]));
		(yyval.pnode)->child->str = (yyvsp[(3) - (8)].str);
		(yyval.pnode)->child->child = (yyvsp[(5) - (8)].pnode);
		(yyval.pnode)->child->next = (yyvsp[(7) - (8)].pnode);
	;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 634 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('+', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 636 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('-', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 638 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('*', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 640 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('/', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 642 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("^", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 644 "psycon.y"
    { (yyval.pnode) = makeFunctionCall("caret", (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 646 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode('@', (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 648 "psycon.y"
    {
		(yyval.pnode) = newAstNode(NODE_INTERPOL, (yyloc));
		(yyval.pnode)->child = (yyvsp[(1) - (5)].pnode);
		(yyvsp[(1) - (5)].pnode)->next = (yyvsp[(3) - (5)].pnode);
		(yyvsp[(3) - (5)].pnode)->next = (yyvsp[(5) - (5)].pnode);
		(yyval.pnode)->LastChild = (yyvsp[(5) - (5)].pnode);
	;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 656 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_SHIFT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 658 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_SHIFT2, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 660 "psycon.y"
    { (yyval.pnode) = makeBinaryOpNode(T_OP_CONCAT, (yyvsp[(1) - (3)].pnode), (yyvsp[(3) - (3)].pnode), (yyloc));;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 662 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 668 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 676 "psycon.y"
    {
		(yyval.pnode) = (yyvsp[(2) - (3)].pnode);
		(yyval.pnode)->type = NODE_INITCELL;
		(yyval.pnode)->line = (yyloc).first_line;
		(yyval.pnode)->column = (yyloc).first_column;
	;}
    break;



/* Line 1455 of yacc.c  */
#line 3221 "psycon.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (pproot, errmsg, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (pproot, errmsg, yymsg);
	  }
	else
	  {
	    yyerror (pproot, errmsg, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, pproot, errmsg);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (pproot, errmsg, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, pproot, errmsg);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, pproot, errmsg);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 684 "psycon.y"


/* Called by yyparse on error. */
void yyerror (AstNode **pproot, char **errmsg, char const *s)
{
  static size_t errmsg_len = 0;
#define ERRMSG_MAX 999
  char msgbuf[ERRMSG_MAX], *p;
  size_t msglen;

  sprintf_s(msgbuf, ERRMSG_MAX, "Line %d, Col %d: %s.\n", yylloc.first_line, yylloc.first_column, s + (strncmp(s, "syntax error, ", 14) ? 0 : 14));
  if ((p=strstr(msgbuf, "$undefined"))) {
	sprintf_s(p, 10, "'%c'(%d)", yychar, yychar);
    strcpy(p+strlen(p), p+10);
  }
  if ((p=strstr(msgbuf, "end of text or ")))
    strcpy(p, p+15);
  if ((p=strstr(msgbuf, " or ','")))
    strcpy(p, p+7);
  msglen = strlen(msgbuf);
  if (ErrorMsg == NULL)
    errmsg_len = 0;
  ErrorMsg = (char *)realloc(ErrorMsg, errmsg_len+msglen+1);
  strcpy_s(ErrorMsg+errmsg_len, msglen+1, msgbuf);
  errmsg_len += msglen;
  *errmsg = ErrorMsg;
}


int getTokenID(const char *str)
{
	size_t len, i;
	len = strlen(str);
	for (i = 0; i < YYNTOKENS; i++) {
		if (yytname[i] != 0
			&& yytname[i][0] == '"'
			&& !strncmp (yytname[i] + 1, str, len)
			&& yytname[i][len + 1] == '"'
			&& yytname[i][len + 2] == 0)
				break;
	}
	if (i < YYNTOKENS)
		return yytoknum[i];
	else
		return T_UNKNOWN;
}


void print_token_value(FILE *file, int type, YYSTYPE value)
{
	if (type == T_ID)
		fprintf (file, "%s", value.str);
	else if (type == T_NUMBER)
		fprintf (file, "%f", value.dval);
}


char *getAstNodeName(AstNode *p)
{
#define NODE_NAME_MAX 99
  static char buf[NODE_NAME_MAX];

  if (!p)
	return NULL;
  switch (p->type) {
  case '=':
    sprintf_s(buf, NODE_NAME_MAX, "[%s=]", p->str);
    break;
  case T_ID:
    sprintf_s(buf, NODE_NAME_MAX, "[%s]", p->str);
    break;
  case T_STRING:
    sprintf_s(buf, NODE_NAME_MAX, "\"%s\"", p->str);
    break;
  case NODE_CALL:
    sprintf_s(buf, NODE_NAME_MAX, "%s()", p->str);
    break;
  case T_NUMBER:
    sprintf_s(buf, NODE_NAME_MAX, "%.1f", p->dval);
    break;
  case NODE_BLOCK:
    sprintf_s(buf, NODE_NAME_MAX, "BLOCK");
    break;
  case NODE_ARGS:
    sprintf_s(buf, NODE_NAME_MAX, "ARGS");
    break;
  case NODE_MATRIX:
    sprintf_s(buf, NODE_NAME_MAX, "MATRIX");
    break;
  case NODE_VECTOR:
    sprintf_s(buf, NODE_NAME_MAX, "VECTOR");
    break;
  case NODE_IDLIST:
    sprintf_s(buf, NODE_NAME_MAX, "ID_LIST");
    break;
  case NODE_EXTRACT:
    sprintf_s(buf, NODE_NAME_MAX, "EXTRACT");
    break;
  case NODE_INITCELL:
    sprintf_s(buf, NODE_NAME_MAX, "INITCELL");
    break;
  case NODE_IXASSIGN:
    sprintf_s(buf, NODE_NAME_MAX, "ASSIGN1");
    break;
  case NODE_INTERPOL:
    sprintf_s(buf, NODE_NAME_MAX, "INTERPOL");
    break;
  default:
    if (YYTRANSLATE(p->type) == 2)
      sprintf_s(buf, NODE_NAME_MAX, "[%d]", p->type);
    else
      sprintf_s(buf, NODE_NAME_MAX, "%s", yytname[YYTRANSLATE(p->type)]);
  }
  return buf;
}


AstNode *makeFunctionCall(char *name, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *nn;

	nn = newAstNode(NODE_CALL, loc);
	nn->str = strdup(name);
	nn->child = first;
	nn->LastChild = first->next = second;
	return nn;
}

AstNode *makeBinaryOpNode(int op, AstNode *first, AstNode *second, YYLTYPE loc)
{
	AstNode *nn;

	nn = newAstNode(op, loc);
	nn->child = first;
	nn->LastChild = first->next = second;
	return nn;
}

AstNode *newAstNode(int type, YYLTYPE loc)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *nn;

  nn = (AstNode *)malloc(sizeof(AstNode));
  if (nn==NULL)
    exit(2);
  memset(nn, 0, sizeof(AstNode));
  nn->type = type;
#ifdef DEBUG
    printf("created node %d: %s\n", ++cnt, getAstNodeName(nn));
#endif
  nn->line = loc.first_line;
  nn->column = loc.first_column;
  return nn;
}


int yydeleteAstNode(AstNode *p, int fSkipNext)
{
#ifdef DEBUG
    static int cnt=0;
#endif
  AstNode *tmp, *next;
  
  if (!p)
	return 0;
#ifdef DEBUG
    printf("deleting node %d: %s\n", ++cnt, getAstNodeName(p));
#endif
  if (p->str)
    free(p->str);
  if (p->child)
    yydeleteAstNode(p->child, 0);
  if (!fSkipNext && p->next) {
	for (tmp=p->next; tmp; tmp=next) {
      next = tmp->next;
      yydeleteAstNode(tmp, 1);
    }
  }
  free(p);
  return 0;
}

