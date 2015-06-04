#include <stdio.h>
#include <dos.h>
#include <alloc.h>

#define   MAXLEN      800
#define   ZWORDCOUNT   59
#define   MAXCHR      128

#define   ERROR        (-1)
#define   ID            0
#define   ZID           1
#define   CHAR_CONST    2
#define   STR_CONST     3
#define   DIG_CONST     4
#define   SPECIAL_CHAR  5
#define   REM           6

#define   unreadchar(ch)    (void)(yes_pred = 1,pred_ch = ch)
#define   NODE      struct node

NODE *add(char *str,int typ);
void del(void);
void print_lex(void);
void errormsg(char *msg);
int is_class1(void);
int is_class2(void);
int is_class3(void);
int is_class4(void);
int is_class5(void);
int is_slash(void);
int is_point(void);
int is_dig10(void);
int is_zero(void);
int is_str(void);
int is_char(void);
int is_id(void);
void lex(void);
void myexit(int exitcode);

char *zword[] = {
"auto",    "break",  "case",    "char",    "continue",
"default", "do",     "double",  "else",    "enum",
"extern",  "float",  "for",     "goto",    "if",
"int",     "long",   "register","return",  "short",
"signed",  "sizeof", "static",  "struct",  "switch",
"typedef", "union",  "unsigned","void",    "while",
"asm",     "cdecl",  "far",     "huge",    "interrupt",
"near",    "pascal", "const",   "volatile","_cs",
"_ds",     "_es",    "_ss",     "_AH",     "_AL",
"_AX",     "_BH",    "_BL",     "_BX",     "_BP",
"_CH",     "_CL",    "_CX",     "_DH",     "_DI",
"_DL",     "_DX",    "_SI",     "_SP"
	       };

char *types[]= {
		"Идентификатор    ",
		"Зарезервировано  ",
		"Символ           ",
		"Строка           ",
		"Число            ",
		"Спец.Символ      "
	       };



 struct node  {
	char str[MAXCHR];
	int count;
	NODE *next;
		     } ;

NODE *pt,*begin[6] = {NULL,NULL,NULL,NULL,NULL,NULL};

int   pred_ch;
int   yes_pred = 0;
char  fileline[MAXLEN];
int   ptr = 0;
char  cur_lex[MAXLEN];
int   i=0, ch = 0;
FILE  *fc, *flex;
/*****************************************
 *          Множества символов
 ****************************************/
char *space_class = " \n\t\v\r\f";
char *dig10_class = "0123456789";
char *dig16_class = "0123456789ABCDEFabcdef";
char *dig8_class  = "01234567";
char *id_beg_class = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm_";
char *id_class = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm0123456789_";

void myexit(int exitcode)
/************************************************
 *
 ***********************************************/
{
  del();
 (void)fcloseall();
  exit(exitcode);
}


void main(int argc,char *argv[])
{
 if (argc!=3)
 {
  printf("Need two parameters\n");
  exit(0);
 }
 if((fc = fopen(argv[1],"rt"))== NULL)
 {
  printf("Error file N1\n");
  myexit(1);
 }
 if((flex = fopen(argv[2],"wt"))== NULL)
 {
  printf("Error file N2\n");
  myexit(2);
 }
 lex();

}

void lex(void)
{
  int lextype;

 while(1)
 {
  i=0;
  ch = readchar();
  if (ch == EOF) print_lex(); /*  */
  else
      if (strchr(space_class,ch) != NULL) continue; /* Если пробелы */
      else
	  if (strchr(id_beg_class,ch) != NULL) lextype = is_id();/* Это id. */
	  else switch(ch)
	       {
		case '\"':
			  lextype = is_str();/* Это строка */
			  break;
		case '\'':
			  lextype = is_char();/* Это символ */
			  break;
		case '0' :
			  lextype = is_zero();/* Это число,нач. с  0 */
			  break;
		 case '1':
		 case '2':
		 case '3':
		 case '4':
		 case '5':
		 case '6':
		 case '7':
		 case '8':
		 case '9':
			  lextype = is_dig10(); /* Это 10-ное число */
			  break;
		case '\\':
		case  '?':
		case ':':
		case ',':
		case '(':
		case ')':
		case '[':
		case ']':
		case '~':
		case ';':
		case '{':
		case '}':
			 lextype = SPECIAL_CHAR;
			 cur_lex[i++] = ch; /* Это спец.символ */
			 cur_lex[i] = '\0';
			 break;
		case '.':
			 lextype = is_point(); /* Это число или операция */
			 break;
		case '-':
			 lextype = is_minus();
			 break;
		case '!':
		case '*':
		case '%':
		case '=':
		case '^':
			 lextype = is_class1();
			 break;
		case '&':
		case '+':
			 lextype = is_class2();
			 break;
		case '|':
			 lextype = is_class3();
			 break;
		case '>':
			 lextype = is_class4();
			 break;
		case '<':
			 lextype = is_class5();
			 break;
		case '/':
			 lextype = is_slash();
			 break;
		default :
			 break;

	       }
	      pt = add(cur_lex,lextype);
	      if (pt!=NULL) begin[lextype] = pt;
		 else errormsg("Out of memory!");

 }
}


int is_id(void)
{

 cur_lex[i++] = ch;
 ch = readchar();
 while(ch != EOF && strchr(id_class,ch) != NULL)
 {
  cur_lex[i++] = ch;
  ch = readchar();
 }
 cur_lex[i] ='\0';
 if (ch != EOF) unreadchar(ch);
 return (is_zw() ? ZID : ID );
}

int is_zw(void)
{
 int j;

  for(j=0;j<ZWORDCOUNT;j++)
    if(strcmp(zword[j],cur_lex)==0)
      return 1;
 return 0;
}


int is_char(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == EOF) errormsg("Не найден конец литеры");
 if (ch == '\\')  i = is_char1();
 else  {
          cur_lex[i++] = ch;
	  ch = readchar();
	  if (ch == EOF || ch !='\'') errormsg("Не найден конец литеры");
	  cur_lex[i++] = ch;
	  cur_lex[i] = '\0';
	}
 return CHAR_CONST;
}

int is_char1(void)
{
 int j;

 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == EOF) errormsg("Не найден конец литеры");
 else if (ch == 'X' || ch == 'x')
    {
     cur_lex[i++] = ch;
     for(j=0;j<2;j++)
       {
	ch = readchar();
	if (ch == EOF) errormsg("Не найден конец литеры");
	else if (ch == '\'') break;
	else if (strchr(dig16_class,ch)==NULL)
				   errormsg("Неизвестная константа литеры");
	     else cur_lex[i++] = ch;
       }
   if (ch != '\'') ch = readchar();
   }
else if (strchr(dig8_class,ch) != NULL)
  {
   cur_lex[i++] = ch;
   for(j=0;j<2;j++)
     {
      ch = readchar();
      if (ch == EOF) errormsg("Не найден конец литеры");
      else if (ch == '\'') break;
      else if (strchr(dig8_class,ch)==NULL)
	     errormsg("Неизвестная константа литеры");
	else cur_lex[i++] = ch;
     }
   if (ch!='\'') ch = readchar();
 }
else
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
if (ch == EOF || ch !='\'') errormsg("Не найден конец литеры");
else {
      cur_lex[i++] = ch;
      cur_lex[i] = '\0';
      return CHAR_CONST;
     }
}

int is_str(void)
{
 int pred;

 cur_lex[i++] = ch;
 ch = readchar();
 pred = ch != '\"';
while(pred)
 while(ch != EOF && ch != '\"' )
 {
  pred = ch =='\\';
  cur_lex[i++] = ch;
  ch = readchar();
 }
if (ch == EOF) errormsg("Не найден конец строки");
cur_lex[i++] = ch;
cur_lex[i] = '\0';
return STR_CONST;
}

int is_zero(void)
{

 cur_lex[i++] = ch;
 ch = readchar();
 if (strchr(dig8_class,ch) != NULL)
  {
   cur_lex[i++] = ch;
   ch = readchar();
   while(ch !=EOF && strchr(dig8_class,ch) != NULL)
    {
     cur_lex[i++] = ch;
     ch = readchar();
    }
  if (ch == 'L' || ch == 'l')
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
 cur_lex[i] = '\0';
 unreadchar(ch);
 return DIG_CONST;
 }
else if (ch == 'x' || ch == 'X')
  {
   cur_lex[i++] = ch;
   ch = readchar();
   while(ch !=EOF && strchr(dig16_class,ch) != NULL)
    {
     cur_lex[i++] = ch;
     ch = readchar();
    }
  if (ch == 'L' || ch == 'l')
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
 cur_lex[i] = '\0';
 unreadchar(ch);
 return DIG_CONST;
 }
else if(ch == '.')
  {
   cur_lex[i++] = ch;
   ch = readchar();
   while(ch != EOF && strchr(dig10_class,ch) != NULL)
    {
     cur_lex[i++] = ch;
     ch = readchar();
    }
   if (ch == 'e' || ch == 'E')
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
  if (ch == '-' || ch == '+')
  {
   cur_lex[i++] = ch;
   ch = readchar();
  }
  while(ch != EOF && strchr(dig10_class,ch) != NULL)
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
 cur_lex[i] = '\0';
 unreadchar(ch);
 return DIG_CONST;
 }
else
 {
  cur_lex[i] = '\0';
  unreadchar(ch);
  return DIG_CONST;
 }

}

int is_dig10(void)
{
  cur_lex[i++] = ch;
  ch = readchar();

  while(strchr(dig10_class,ch) != NULL)
   {
   cur_lex[i++] = ch;
   ch = readchar();
   }
 if (ch == 'l' || ch == 'L')
  {
   cur_lex[i++] = ch;
   cur_lex[i] = '\0';
   return  DIG_CONST;
  }
 if (ch == '.')
  {
   cur_lex[i++] = ch;
   ch = readchar();
   while(ch != EOF && strchr(dig10_class,ch) != NULL)
    {
     cur_lex[i++] = ch;
     ch = readchar();
    }
  }
 if (ch == 'e' || ch == 'E')
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
  if (ch == '-' || ch == '+')
  {
   cur_lex[i++] = ch;
   ch = readchar();
  }
  while(ch != EOF && strchr(dig10_class,ch) != NULL)
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
 cur_lex[i] = '\0';
 unreadchar(ch);
 return DIG_CONST;
}

int is_point(void)
{
 cur_lex[i++] =ch;
 ch = readchar();
 if (ch == EOF || strchr(dig10_class,ch) == NULL)
  {
   cur_lex[i] = '\0';
   unreadchar(ch);
   return SPECIAL_CHAR;
  }
  while(ch != EOF && strchr(dig10_class,ch) != NULL)
    {
     cur_lex[i++] = ch;
     ch = readchar();
    }
 if (ch == 'e' || ch == 'E')
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
  if (ch == '-' || ch == '+')
  {
   cur_lex[i++] = ch;
   ch = readchar();
  }
  while(ch != EOF && strchr(dig10_class,ch) != NULL)
   {
    cur_lex[i++] = ch;
    ch = readchar();
   }
 cur_lex[i] = '\0';
 unreadchar(ch);
 return DIG_CONST;
}

int is_class1(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch != '=')  unreadchar(ch);
	   else  cur_lex[i++] = ch;
 cur_lex[i] = '\0';
 return SPECIAL_CHAR;
}

int is_class2(void)
{
 int c = ch;

 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == '=' || ch == c )   cur_lex[i++] = ch;
		      else    unreadchar(ch);
 cur_lex[i] = '\0';
 return SPECIAL_CHAR;

}

int is_minus(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == '=' || ch == '>' || ch == '-' )   cur_lex[i++] = ch;
				      else    unreadchar(ch);
 cur_lex[i] = '\0';
 return SPECIAL_CHAR;
}

int is_class3(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == '=' || ch == '|' )   cur_lex[i++] = ch;
				      else    unreadchar(ch);
 cur_lex[i] = '\0';
 return SPECIAL_CHAR;
}

int is_class4(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == '=')
   {
     cur_lex[i++] = ch;
     cur_lex[i] = '\0';
     return SPECIAL_CHAR;
   }
 if(ch == '>')
  {
    cur_lex[i++] = ch;
    ch = readchar();
    if (ch == '=' )   cur_lex[i++] = ch;
	      else    unreadchar(ch);
    cur_lex[i] = '\0';
    return SPECIAL_CHAR;
  }
  unreadchar(ch);
  cur_lex[i] = '\0';
  return SPECIAL_CHAR;
}

int is_class5(void)
{
 cur_lex[i++] = ch;
 ch = readchar();
 if (ch == '=' || ch == '>')
   {
     cur_lex[i++] = ch;
     cur_lex[i] = '\0';
     return SPECIAL_CHAR;
   }
 if(ch == '<')
  {
    cur_lex[i++] = ch;
    ch = readchar();
    if (ch == '=' )   cur_lex[i++] = ch;
	      else    unreadchar(ch);
    cur_lex[i] = '\0';
    return SPECIAL_CHAR;
  }
  unreadchar(ch);
  cur_lex[i] = '\0';
  return SPECIAL_CHAR;
}

int is_slash(void)
{
 int in_rem;

 cur_lex[i++] = ch;
 ch = readchar();
 if (ch != '=' && ch != '*')
  {
   unreadchar(ch);
   cur_lex[i] = '\0';
   return SPECIAL_CHAR;
  }
 if (ch == '=')
 {
  cur_lex[i++] = ch;
  cur_lex[i] = '\0';
  return SPECIAL_CHAR;
 }
 ch = readchar();
 in_rem = 1;
 while(in_rem)
  {
  while(ch != EOF && ch != '*')
     ch = readchar();
  if (ch == EOF) errormsg("Нет конца коментария");
  ch = readchar();
  in_rem = ch != '/';
  }
 return REM;
}



int readchar(void)
{
 char *c;

 if (yes_pred)
 {
  yes_pred = 0;
  return pred_ch;
 }
 if (!fileline[ptr])
 {
  c = fgets(fileline,MAXLEN,fc);
  ptr = 0;
  if (c == NULL) return EOF;
  printf("%s",fileline);
 }
 return fileline[ptr++];
}

void errormsg(char *msg)
{
 fprintf(flex,"%s\n",msg);
 fprintf(flex,"%s\n",fileline);
 printf("%c%s%c\n",7,msg,7);
 printf("%s\n",fileline);
 myexit(1);
}





NODE *add(char *str,int typ)
{
  NODE *q,*p;

  for(p = begin[typ];p != NULL;p=p->next)
    if (strcmp(p->str,str)==0)
     {
      (p->count)++;
      return begin[typ];
     }

  if ((q = (NODE *)malloc(sizeof(NODE)))==NULL) return NULL;
  (void)strncpy(q->str,str,MAXCHR);
  q->count = 1;
  q->next = begin[typ];
  return q;
}

void del(void)
{
 NODE *q;
 int j;

 for (j=0;j<6;j++)
 {
   for(;begin[j] != NULL;begin[j] = q)
   {
    q = begin[j]->next;
    free(begin[j]);
   }
 begin[j] = NULL;
 }
}

void print_lex(void)
{
 int i;
 NODE *q;

 for(i=0;i<6;i++)
 {
   fprintf(flex,"%s\n",types[i]);
  for(q=begin[i];q != NULL;q=q->next)
      fprintf(flex,"%40s  %10d\n",q->str,q->count);
 }
 myexit(0);
}

