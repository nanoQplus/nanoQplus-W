/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_ZCONF_ZCONF_TAB_H_INCLUDED
# define YY_ZCONF_ZCONF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int zconfdebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_MAINMENU = 258,
    T_MENU = 259,
    T_ENDMENU = 260,
    T_SOURCE = 261,
    T_CHOICE = 262,
    T_ENDCHOICE = 263,
    T_COMMENT = 264,
    T_CONFIG = 265,
    T_MENUCONFIG = 266,
    T_HELP = 267,
    T_HELPTEXT = 268,
    T_IF = 269,
    T_ENDIF = 270,
    T_DEPENDS = 271,
    T_REQUIRES = 272,
    T_OPTIONAL = 273,
    T_PROMPT = 274,
    T_DEFAULT = 275,
    T_TRISTATE = 276,
    T_DEF_TRISTATE = 277,
    T_BOOLEAN = 278,
    T_DEF_BOOLEAN = 279,
    T_STRING = 280,
    T_INT = 281,
    T_HEX = 282,
    T_WORD = 283,
    T_WORD_QUOTE = 284,
    T_UNEQUAL = 285,
    T_EOF = 286,
    T_EOL = 287,
    T_CLOSE_PAREN = 288,
    T_OPEN_PAREN = 289,
    T_ON = 290,
    T_SELECT = 291,
    T_RANGE = 292,
    T_OR = 293,
    T_AND = 294,
    T_EQUAL = 295,
    T_NOT = 296
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 35 "zconf.y" /* yacc.c:1909  */

	int token;
	char *string;
	struct symbol *symbol;
	struct expr *expr;
	struct menu *menu;

#line 104 "zconf.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE zconflval;

int zconfparse (void);

#endif /* !YY_ZCONF_ZCONF_TAB_H_INCLUDED  */
