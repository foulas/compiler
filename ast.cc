
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

#include<iostream>
#include<fstream>
#include<iomanip>

using namespace std;

#include"user-options.hh"
#include"error-display.hh"
#include"local-environment.hh"

#include"symbol-table.hh"
#include"ast.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast(int line)
{
	report_internal_error("Should not reach, Ast : check_ast");
}

Data_Type Ast::get_data_type()
{
	report_internal_error("Should not reach, Ast : get_data_type");
}

void Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	report_internal_error("Should not reach, Ast : print_value");
}

Eval_Result & Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	report_internal_error("Should not reach, Ast : get_value_of_evaluation");
}

void Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	report_internal_error("Should not reach, Ast : set_value_of_evaluation");
}

////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
}

Assignment_Ast::~Assignment_Ast()
{
	delete lhs;
	delete rhs;
}

Data_Type Assignment_Ast::get_data_type()
{
	return node_data_type;
}

bool Assignment_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Assignment statement data type not compatible", line);
}

void Assignment_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Asgn:\n";

	file_buffer << AST_NODE_SPACE"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << AST_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")\n";
}

Eval_Result & Assignment_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	if (result.is_variable_defined() == false)
		report_error("Variable should be defined to be on rhs", NOLINE);

	lhs->set_value_of_evaluation(eval_env, result);

	// Print the result

	print_ast(file_buffer);

	lhs->print_value(eval_env, file_buffer);

	return result;
}
/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry)
{
	variable_name = name;
	variable_symbol_entry = var_entry;
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	return variable_symbol_entry.get_data_type();
}

void Name_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << "Name : " << variable_name;
}

void Name_Ast::print_value(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result_Value * loc_var_val = eval_env.get_variable_value(variable_name);
	Eval_Result_Value * glob_var_val = interpreter_global_table.get_variable_value(variable_name);
	// cerr << loc_var_val->float_get_value() << endl;
	// if (loc_var_val->get_result_enum()==float_result) cout << "Yep";

	file_buffer << "\n" << AST_SPACE << variable_name << " : ";

	if (!eval_env.is_variable_defined(variable_name) && !interpreter_global_table.is_variable_defined(variable_name))
		file_buffer << "undefined";

	else if (eval_env.is_variable_defined(variable_name) && loc_var_val != NULL)
	{
		if (loc_var_val->get_result_enum() == int_result)
			file_buffer << loc_var_val->get_value() << "\n";
		else if (loc_var_val->get_result_enum() == float_result)
			file_buffer << fixed<<setprecision(2)<< loc_var_val->float_get_value() << "\n";
		else
			report_internal_error("Result type can only be int and float");
	}

	else
	{
		if (glob_var_val->get_result_enum() == int_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << glob_var_val->get_value() << "\n";
		}
		else if (glob_var_val->get_result_enum() == float_result)
		{
			if (glob_var_val == NULL)
				file_buffer << "0\n";
			else
				file_buffer << fixed<<setprecision(2)<< glob_var_val->float_get_value() << "\n";
		}
		else
			report_internal_error("Result type can only be int and float");
	}
	file_buffer << "\n";
}

Eval_Result & Name_Ast::get_value_of_evaluation(Local_Environment & eval_env)
{
	if (eval_env.does_variable_exist(variable_name))
	{
		Eval_Result * result = eval_env.get_variable_value(variable_name);
		return *result;
	}

	Eval_Result * result = interpreter_global_table.get_variable_value(variable_name);
	return *result;
}

void Name_Ast::set_value_of_evaluation(Local_Environment & eval_env, Eval_Result & result)
{
	Eval_Result_Value * i;
	if (result.get_result_enum() == int_result)
	{
		i = new Eval_Result_Value_Int();
	 	i->set_value(result.get_value());
	}

	if (result.get_result_enum() == float_result)
	{
		i = new Eval_Result_Value_Float();
	 	i->float_set_value(result.float_get_value());
	}
	// cout << "Check " << i->float_get_value() <<endl;

	if (eval_env.does_variable_exist(variable_name))
		eval_env.put_variable_value(*i, variable_name);
	else
		interpreter_global_table.put_variable_value(*i, variable_name);
}

Eval_Result & Name_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	return get_value_of_evaluation(eval_env);
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type)
{
	constant = number;
	node_data_type = constant_data_type;
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print_ast(ostream & file_buffer)
{
	file_buffer << "Num : ";

	if(node_data_type==float_data_type)
		file_buffer<<fixed<<setprecision(2)<<constant;
	else
		file_buffer<<constant;
}

template <class DATA_TYPE>
Eval_Result & Number_Ast<DATA_TYPE>::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if (node_data_type == int_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Int();
		result.set_value(constant);

		return result;
	}
	if(node_data_type==float_data_type)
	{
		Eval_Result & result = *new Eval_Result_Value_Float();
		// cout<<"kri "<<constant<<endl;
		result.float_set_value(constant);
		// cout<<"krish "<<result.float_get_value()<<endl;
		return result;		
	}
}

///////////////////////////////////////////////////////////////////////////////

Return_Ast::Return_Ast()
{}

Return_Ast::~Return_Ast()
{}

void Return_Ast::print_ast(ostream & file_buffer)
{
	file_buffer << AST_SPACE << "Return <NOTHING>\n";
}

Eval_Result & Return_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_Return();
	file_buffer<< AST_SPACE << "Return <NOTHING>\n";
	return result;
}

template class Number_Ast<int>;
template class Number_Ast<float>;
///////////////////////////////////////////////////////////////////////////////
Relational_Expr_Ast::Relational_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs,string temp_op)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	op=temp_op;
}

Relational_Expr_Ast::~Relational_Expr_Ast()
{
	delete lhs;
	delete rhs;
	delete &op;
}

Data_Type Relational_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Relational_Expr_Ast::check_ast(int line)
{
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Relational Expression Statement data type not compatible", line);
}

void Relational_Expr_Ast::print_ast(ostream & file_buffer)
{
	file_buffer <<"\n"<< COND_SPACE << "Condition: "<<op<<"\n";

	file_buffer << COND_NODE_SPACE<<"LHS (";
	lhs->print_ast(file_buffer);
	file_buffer << ")\n";

	file_buffer << COND_NODE_SPACE << "RHS (";
	rhs->print_ast(file_buffer);
	file_buffer << ")";
}

Eval_Result & Relational_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result1 = lhs->evaluate(eval_env, file_buffer);
	Eval_Result & result2 = rhs->evaluate(eval_env, file_buffer);

	Eval_Result & result = *new Eval_Result_Value_Int();

	if(op.compare("GT")==0) {
		result.set_value(result1.get_value()>result2.get_value());
	}
	else if(op.compare("GE")==0) {
		result.set_value(result1.get_value()>=result2.get_value());
	}
	else if(op.compare("EQ")==0) {
		result.set_value(result1.get_value()==result2.get_value());
	}
	else if(op.compare("NE")==0) {
		result.set_value(result1.get_value()!=result2.get_value());
	}
	else if(op.compare("LT")==0) {
		result.set_value(result1.get_value()<result2.get_value());
	}
	else if(op.compare("LE")==0) {
		result.set_value(result1.get_value()<=result2.get_value());
	}
	else {
		;
	}
	// lhs->set_value_of_evaluation(eval_env, result);

	// // Print the result

	// print_ast(file_buffer);

	// lhs->print_value(eval_env, file_buffer);

	return result;
}

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Arithmetic_Expr_Ast::Arithmetic_Expr_Ast(Ast * temp_lhs,int temp_op)
// {
// 	lhs = temp_lhs;
// 	op='*';
// 	op+=temp_op;
// 	rhs=NULL;
// }

Arithmetic_Expr_Ast::Arithmetic_Expr_Ast(Ast * temp_lhs, Ast * temp_rhs,int temp_op)
{
	lhs = temp_lhs;
	rhs = temp_rhs;
	// op='*';
	op=temp_op;
}

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast()
{
	delete lhs;
	delete rhs;
	delete &op;
}

Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	return node_data_type;
}

bool Arithmetic_Expr_Ast::check_ast(int line)
{
	if(rhs==NULL) {
		if(op==0)
			node_data_type=lhs->get_data_type();
		else if(op==1)
			node_data_type=float_data_type;
		else if(op==2)
			node_data_type=int_data_type;
		else
			;
		return true;
	}
	if (lhs->get_data_type() == rhs->get_data_type())
	{
		node_data_type = lhs->get_data_type();
		return true;
	}

	report_error("Arithmetic Expression statement data type not compatible", line);
}

void Arithmetic_Expr_Ast::print_ast(ostream & file_buffer)
{
	if(rhs==NULL) {
		if(op==0) {
			file_buffer <<"\n"<< ARITH_SPACE << "Arith: UMINUS\n";
			file_buffer << ARITH_NODE_SPACE<<"LHS (";
			lhs->print_ast(file_buffer);
			file_buffer << ")";
		}
		else {
			lhs->print_ast(file_buffer);
		}
	}
	else {
		file_buffer <<"\n"<< ARITH_SPACE << "Arith: ";

		if(op==0)
			file_buffer<<"MULT";
		else if(op==2)
			file_buffer<<"PLUS";
		else if(op==3)
			file_buffer<<"MINUS";
		else if(op==1)
			file_buffer<<"DIV";
		else
			;
		file_buffer<<"\n";

		file_buffer << ARITH_NODE_SPACE<<"LHS (";
		lhs->print_ast(file_buffer);
		file_buffer << ")\n";

		file_buffer << ARITH_NODE_SPACE << "RHS (";
		rhs->print_ast(file_buffer);
		file_buffer << ")";
	}

}

Eval_Result & Arithmetic_Expr_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	if(rhs==NULL) {
		if(op==0) {
			Eval_Result & result=lhs->evaluate(eval_env,file_buffer);

			if(result.get_result_enum()==int_result) {
				Eval_Result & result1=*new Eval_Result_Value_Int();
				result1.set_value(-result.get_value());
				return result1;
			}
			else if(result.get_result_enum()==float_result) {
				Eval_Result & result1=*new Eval_Result_Value_Float();
				result1.float_set_value(-result.float_get_value());
				return result1;
			}
			else
				;
			// return result;
		}
		if(op==1) {
			Eval_Result & result=lhs->evaluate(eval_env,file_buffer);
			if(result.get_result_enum()==float_result) {
				Eval_Result & result1=*new Eval_Result_Value_Float();
				result1.float_set_value(result.float_get_value()); 
				return result1;
			}
			else {
				Eval_Result & result1=* new Eval_Result_Value_Float();
				result1.float_set_value((float)result.get_value());
				return result1;
			}
		}
		if(op==2) {
			Eval_Result & result=lhs->evaluate(eval_env,file_buffer);
			
			if(result.get_result_enum()==int_result) {
				Eval_Result & result1=* new Eval_Result_Value_Int();
				result1.set_value(result.get_value());
				return result1;
			}
			else {
				Eval_Result & result1=* new Eval_Result_Value_Int();
				result1.set_value((int)result.float_get_value());
				return result1;
			}
		}

	}
	else {
		Eval_Result & result1 = lhs->evaluate(eval_env, file_buffer);
		Eval_Result & result2 = rhs->evaluate(eval_env, file_buffer);
		Eval_Result & result = *new Eval_Result_Value_Int();
		
		if(result1.get_result_enum()==int_result && result2.get_result_enum()==int_result) {
			Eval_Result & result = *new Eval_Result_Value_Int();
			if(op==0)
				result.set_value(result1.get_value() * result2.get_value());
			else if(op==1)
				result.set_value(result1.get_value() / result2.get_value());
			else if(op==2)
				result.set_value(result1.get_value() + result2.get_value());
			else if(op==3)
				result.set_value(result1.get_value() - result2.get_value());
			else
				;
			return result;
		}
		else if(result1.get_result_enum()==float_result && result2.get_result_enum()==float_result) {
			Eval_Result & result = *new Eval_Result_Value_Float();
			if(op==0)
				result.float_set_value(result1.float_get_value() * result2.float_get_value());
			else if(op==1)
				result.float_set_value(result1.float_get_value() / result2.float_get_value());
			else if(op==2)
				result.float_set_value(result1.float_get_value() + result2.float_get_value());
			else if(op==3)
				result.float_set_value(result1.float_get_value() - result2.float_get_value());
			else
				;
			return result;
		}
		else {
			report_internal_error("The operands should be of the same type.");		
		}
	}


	// // Print the result

	// print_ast(file_buffer);

	// lhs->print_value(eval_env, file_buffer);

	// return result;
}

///////////////////////////////////////////////////////////////////////////////
Goto_Ast::Goto_Ast(int num)
{
	bb_num=num;
}

Goto_Ast::~Goto_Ast()
{
	delete &bb_num;
}


void Goto_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<< AST_SPACE << "Goto statement:\n";

	file_buffer << AST_NODE_SPACE<<"Successor: "<<bb_num<<endl;
}


Eval_Result & Goto_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{
	Eval_Result & result = *new Eval_Result_Value_BB();
	result.set_value(bb_num);
	print_ast(file_buffer);
	file_buffer<< AST_SPACE << "GOTO (BB "<<bb_num<<")\n";

	// file_buffer << AST_SPACE << "GOTO  "
	return result;

}

///////////////////////////////////////////////////////////////////////////////

If_Ast::If_Ast(Ast* temp_comp,int temp_true,int temp_false)
{
	comp_exp=temp_comp;
	true_bb=temp_true;
	false_bb=temp_false;
}

If_Ast::~If_Ast()
{
	delete comp_exp;
	delete &true_bb;
	delete &false_bb;
}


void If_Ast::print_ast(ostream & file_buffer)
{
	file_buffer<< AST_SPACE << "If_Else statement:";
	comp_exp->print_ast(file_buffer);
	file_buffer <<endl<< AST_NODE_SPACE<<"True Successor: "<<true_bb<<endl;
	file_buffer << AST_NODE_SPACE<<"False Successor: "<<false_bb<<endl;
}

Eval_Result & If_Ast::evaluate(Local_Environment & eval_env, ostream & file_buffer)
{

	Eval_Result & result = comp_exp->evaluate(eval_env, file_buffer);

	int ret = result.get_value();

	int ans;
	print_ast(file_buffer);
	
	if(ret){
		file_buffer<<AST_SPACE<<"Condition True : Goto (BB "<<true_bb<<")\n";
		ans = true_bb;
	}
	else{
		file_buffer<<AST_SPACE<<"Condition False : Goto (BB "<<false_bb<<")\n";
		ans = false_bb;
	}
	
	Eval_Result & result1 = *new Eval_Result_Value_BB();
	result1.set_value(ans);


	// Condition True : Goto (BB 3)	
	return result1;

	// Eval_Result & result = rhs->evaluate(eval_env, file_buffer);

	// if (result.is_variable_defined() == false)
	// 	report_error("Variable should be defined to be on rhs", NOLINE);

	// lhs->set_value_of_evaluation(eval_env, result);

	// // Print the result

	// print_ast(file_buffer);

	// lhs->print_value(eval_env, file_buffer);

	// return result;
}

