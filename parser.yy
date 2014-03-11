
/*********************************************************************************************

                                cfglp : A CFG Language Processor
                                --------------------------------

           About:

           Implemented   by  Tanu  Kanvar (tanu@cse.iitb.ac.in) and Uday
           Khedker    (http://www.cse.iitb.ac.in/~uday)  for the courses
           cs302+cs306: Language  Processors  (theory and  lab)  at  IIT
           Bombay.

           Release  date  Jan  15, 2013.  Copyrights  reserved  by  Uday
           Khedker. This  implemenation  has been made  available purely
           for academic purposes without any warranty of any kind.

           Documentation (functionality, manual, and design) and related
           tools are  available at http://www.cse.iitb.ac.in/~uday/cfglp


***********************************************************************************************/

%scanner ../scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	int integer_value;
	float float_value;
	Data_Type data_type;
	std::string * string_value;
	list<Ast *> * ast_list;
	Ast * ast;
	Symbol_Table * symbol_table;
	Symbol_Table_Entry * symbol_entry;
	Basic_Block * basic_block;
	list<Basic_Block *> * basic_block_list;
	Procedure * procedure;
};



%token <integer_value> INTEGER_NUMBER
%token <integer_value> BASIC_BLOCK
%token <float_value> FLOAT_NUMBER
%token <string_value> NAME
%token <data_type> RETURN
%token <data_type> INTEGER
%token <data_type> FLOAT 
%token <data_type> DOUBLE
%token <data_type> VOID
%token IF
%token ELSE
%token GOTO
%token ASSIGN_OP
%left <string_value> NE EQ
%left <string_value> LT LE GT GE
%left '+' '-'
%left '*' '/'


%type <symbol_table> declaration_statement_list
%type <symbol_entry> declaration_statement
%type <basic_block_list> basic_block_list
%type <basic_block> basic_block
%type <ast_list> executable_statement_list
%type <ast_list> assignment_statement_list
%type <ast> assignment_statement
%type <ast> comparision_expression
%type <ast> goto_statement
%type <ast> if_block
%type <ast> variable
%type <ast> constant
%type <ast> arith_expression
%type <data_type> type_declaration

%start program

%%

program:
	declaration_statement_list
	procedure_decls
	procedure_list
|
	procedure_decls
	procedure_list
;

procedure_list:

	procedure_name
	{
		#if 0
		return_statement_used_flag = false;				// No return statement in the current procedure till now
		#endif
	}
	procedure_body
	{	
		#if 0
		
		program_object.set_procedure_map(*current_procedure);
		#endif
	}
 procedure_list
|

;

procedure_decls:
	type_declaration procedure_name ';' procedure_decls
	{
	
	}
|

;

procedure_name:
	NAME '(' argument_list ')'
	{	
		#if 0
		current_procedure = new Procedure(void_data_type, *$1);
		#endif
	}
|
	NAME '(' ')'
;

argument_list:
	argument_list ',' argument
|
	argument

;

argument:
	type_declaration NAME
;


procedure_body:
	'{' declaration_statement_list
	{	
		#if 0
		current_procedure->set_local_list(*$2);
		delete $2;
		#endif
	}
	basic_block_list '}'
	{
		#if 0
		
		// if (return_statement_used_flag == false)
		// {
		// 	int line = get_line_number();
		// 	report_error("Atleast 1 basic block should have a return statement", line);
		// }

		current_procedure->set_basic_block_list(*$4);

		delete $4;
		#endif
	}
|
	'{' basic_block_list '}'
	{
		#if 0
		// if (return_statement_used_flag == false)
		// {
		// 	int line = get_line_number();
		// 	report_error("Atleast 1 basic block should have a return statement", line);
		// }

		current_procedure->set_basic_block_list(*$2);

		delete $2;
		#endif
	}
;

declaration_statement_list:
	declaration_statement
	{
		#if 0
		int line = get_line_number();
		program_object.variable_in_proc_map_check($1->get_variable_name(), line);

		string var_name = $1->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		$$ = new Symbol_Table();
		$$->push_symbol($1);
		#endif
	}
|
	declaration_statement_list declaration_statement
	{
		#if 0
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		int line = get_line_number();
		program_object.variable_in_proc_map_check($2->get_variable_name(), line);

		string var_name = $2->get_variable_name();
		if (current_procedure && current_procedure->get_proc_name() == var_name)
		{
			int line = get_line_number();
			report_error("Variable name cannot be same as procedure name", line);
		}

		if ($1 != NULL)
		{
			if($1->variable_in_symbol_list_check(var_name))
			{
				int line = get_line_number();
				report_error("Variable is declared twice", line);
			}

			$$ = $1;
		}

		else
			$$ = new Symbol_Table();

		$$->push_symbol($2);
		#endif
	}
;

declaration_statement:
	type_declaration NAME ';'
	//need to change this.
	{
		#if 1
		$$ = new Symbol_Table_Entry(*$2, $0);

		delete $2;
		#endif
	}
;

type_declaration:
	INTEGER 
	{
		$$=int_data_type;
	}
|
	FLOAT
	{
		$$=float_data_type;
	}
|
	DOUBLE
	{
		$$=float_data_type;
	}
|
	VOID
	{
		$$=void_data_type;
	}
;
basic_block_list:
	basic_block_list basic_block
	{
		#if 0
		if (!$2)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		bb_strictly_increasing_order_check($$, $2->get_bb_number());

		$$ = $1;
		$$->push_back($2);
		#endif
	}
|
	basic_block
	{
		#if 0
		if (!$1)
		{
			int line = get_line_number();
			report_error("Basic block doesn't exist", line);
		}

		$$ = new list<Basic_Block *>;
		$$->push_back($1);
		#endif
	}
	
;

basic_block:
	BASIC_BLOCK ':' executable_statement_list
	{
		#if 0
		if ($3 != NULL)
		$$ = new Basic_Block($1, *$3);
		else
		{
			list<Ast *> * ast_list = new list<Ast *>;
			$$ = new Basic_Block($1, *ast_list);
		}

		delete $3;
				
		#endif
	}
;

executable_statement_list:
	assignment_statement_list
	{
		#if 0
		$$ = $1;
		#endif
	}
|
	assignment_statement_list RETURN return_value ';'
	{
		#if 0
		Ast * ret = new Return_Ast();

		return_statement_used_flag = true;					// Current procedure has an occurrence of return statement

		if ($1 != NULL)
			$$ = $1;

		else
			$$ = new list<Ast *>;

		$$->push_back(ret);
		#endif
	}
;

return_value:
	comparision_expression
|

;

assignment_statement_list:
	{
		#if 0
		$$ = NULL;
		#endif
	}
|
	assignment_statement_list assignment_statement
	{
		#if 0
		if ($1 == NULL)
			$$ = new list<Ast *>;

		else
			$$ = $1;

		$$->push_back($2);
		#endif
	}
;

/*assignment statements and if clauses*/

assignment_statement:
	variable ASSIGN_OP comparision_expression ';'
	{
		#if 0
		
		$$=new Assignment_Ast($1,$3);
		$$->check_ast(get_line_number());
		
		#endif

	}
|
	if_block
	{
		#if 0
		$$ = $1;
		#endif
	}
|
	goto_statement	
	{
		#if 0
		$$ = $1;
		#endif
	}
|
	func_call ';'
;

/* if clause */

if_block:

	IF '(' comparision_expression ')'
	GOTO BASIC_BLOCK ';'
	ELSE
	GOTO BASIC_BLOCK ';'
	{
		#if 0
		$$ = new If_Ast($3, $6, $10);
		#endif
	}
;

goto_statement:
	
	GOTO BASIC_BLOCK ';'
	{	
		#if 0
		$$ = new Goto_Ast($2);
		#endif
	}
;

comparision_expression:
	
	arith_expression
	{
		#if 0
		$$= $1;
		#endif
	}
|	
	
	comparision_expression GT comparision_expression
	{
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
|
	comparision_expression LT comparision_expression
	{
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
|
	comparision_expression GE comparision_expression
	{	
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
|
	comparision_expression LE comparision_expression
	{
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
|
	comparision_expression EQ comparision_expression
	{
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
|
	comparision_expression NE comparision_expression
	{
		#if 0
		$$=new Relational_Expr_Ast($1,$3,*$2);
		$$->check_ast(get_line_number());
		#endif
	}
;

arith_expression:
	
	'-' atomic_expr
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($2,NULL,0);
		$$->check_ast(get_line_number());
		#endif
	}
|
	'-' '(' comparision_expression ')'
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($3,NULL,0);
		$$->check_ast(get_line_number());
		#endif
	}
|
	'(' type_declaration ')' atomic_expr 
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($4,NULL,1);
		$$->check_ast(get_line_number());
		#endif
	}
|
	'(' type_declaration ')' '(' comparision_expression ')'
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($5,NULL,1);
		$$->check_ast(get_line_number());
		#endif
	}
|
	'(' comparision_expression ')'
	{
		#if 0
		$$=$2;
		#endif
	}

|
	atomic_expr
	{	
		#if 0
		$$ = $1;
		#endif
	}
|
	arith_expression '*' arith_expression
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($1,$3,0);
		$$->check_ast(get_line_number());
		#endif
	}

| 
	arith_expression '/' arith_expression
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($1,$3,1);
		$$->check_ast(get_line_number());
		#endif
	}

|
	arith_expression '+' arith_expression
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($1,$3,2);
		$$->check_ast(get_line_number());
		#endif
	}

|
	arith_expression '-' arith_expression
	{
		#if 0
		$$=new Arithmetic_Expr_Ast($1,$3, 3);
		$$->check_ast(get_line_number());
		#endif
	}

;

atomic_expr:
	variable
|
	constant
|
	func_call
;

func_call:
	NAME '(' pass_variable_list ')'
|
	NAME '(' ')'
;

pass_variable_list:
	comparision_expression ',' pass_variable_list
|
	comparision_expression
;


variable:
	NAME
	{
		#if 0
		Symbol_Table_Entry var_table_entry;

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = current_procedure->get_symbol_table_entry(*$1);

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = program_object.get_symbol_table_entry(*$1);

		else
		{
			int line = get_line_number();
			report_error("Variable has not been declared", line);
		}

		$$ = new Name_Ast(*$1, var_table_entry);

		delete $1;
		#endif
	}
;

constant:
	INTEGER_NUMBER
	{
		#if 0
		$$ = new Number_Ast<int>($1, int_data_type);
		#endif
	}
|
	FLOAT_NUMBER
	{
		#if 0
		$$= new Number_Ast<float> ($1,float_data_type);
		#endif
	}
;
