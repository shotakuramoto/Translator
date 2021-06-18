// convert Hack VM Code into Hack Assembler

#include "iobuffer.h"
#include "tokeniser.h"
#include "assembler-internal.h"

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

//this global variable created by me
string cl = "";         //this string holds the name of the class

//this holds the name of the function
string funName = "";
string className = "";
string cf = "";
int address = 0;

// translate vm operator command into assembly language
// having a switch statement to check the operators and run the code
//we do the switch statment and run the corresponding hack assembly code
static void translate_vm_operator(TokenKind the_op)
{
    start_of_vm_operator_command(the_op) ;

    // we do switch statement for the type of the variables
    switch (the_op) {

        //when it is or operator
        //the basic operation for the milestone operator will not differ by much
        //however, the last line will have different operator corresponding to the operator
        case tk_or:
            output_assembler ("@SP");
            output_assembler ("AM=M-1");
            output_assembler ("D=M");
            output_assembler ("A=A-1");
            output_assembler ("M=D|M");             //this has operator
            break;

        //when it is add operator
        case tk_add:
            output_assembler ("@SP");
            output_assembler ("AM=M-1");
            output_assembler ("D=M");
            output_assembler ("A=A-1");
            output_assembler ("M=D+M");             //this has add operator
            break;

        //when it is not operator
        case tk_not:
            output_assembler ("@SP");
            output_assembler ("A=M-1");
            output_assembler ("M=!M");              //we have not operator
            break;

        //when it is sub operator
        case tk_sub:
            output_assembler ("@SP");
            output_assembler ("AM=M-1");
            output_assembler ("D=M");
            output_assembler ("A=A-1");
            output_assembler ("M=M-D");             //we use subtraction operator
            break;

        //when it is and operator
        case tk_and:
            output_assembler ("@SP");
            output_assembler ("AM=M-1");
            output_assembler ("D=M");
            output_assembler ("A=A-1");
            output_assembler ("M=D&M");             //we use the and operator
            break;

        //when it is negate
        case tk_neg:
            output_assembler("@SP");
            output_assembler("A=M-1");
            output_assembler("M=-M");               //we neagte the variable by -
            break;

        //when the operator is return.
        //To return the value, we use different segments
        //we will use same operation for pop in each segments as well
        case tk_return:
            output_assembler("@LCL");               //local segment
            output_assembler("D=M");
            output_assembler("@5");
            output_assembler("A=D-A");
            output_assembler("D=M");

            output_assembler("@R14");               //using the spare register for call
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");

            output_assembler("@ARG");               //push the argument using  the similar code
            output_assembler("A=M");
            output_assembler("M=D");
            output_assembler("D=A+1");

            output_assembler("@R15");                //using the spare register for call
            output_assembler("M=D");
            output_assembler("@LCL");
            output_assembler("D=M");

            output_assembler("@SP");                   // decrement stack poitner and change the pointer
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");

            output_assembler("@THAT");                  //push to the THAT register
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");

            output_assembler("@THIS");                  //pop the THAT register as well
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");

            output_assembler("@ARG");
            output_assembler("M=D");
            output_assembler("@SP");
            output_assembler("AM=M-1");
            output_assembler("D=M");

            output_assembler("@LCL");
            output_assembler("M=D");
            output_assembler("@R15");
            output_assembler("D=M");

            output_assembler("@SP");
            output_assembler("M=D");
            output_assembler("@R14");
            output_assembler("A=M");
            output_assembler("0;JMP");
        default:;
    }
    end_of_vm_command() ;
}

// translate vm jump command into assembly language
// checks what it needs to do
// jump, jump to the address
// if goto, do the if statement and jumpt to the address
//lavel, have a label, and store in case of use
static void translate_vm_jump(TokenKind jump, string label)
{
    start_of_vm_jump_command(jump,label) ;

    //check the token kind and run the instruction
    if (jump == tk_goto)
    {
    }
    else if (jump == tk_if_goto)
    {
    }
    //we gonna create the label
    else if (jump == tk_label)
    {
    }
    //we have to create a label with . and $

    end_of_vm_command() ;
}

// translate vm call or function command into assembly language
//when it is function, we prepare stack space for the n arguments
//when ti is call, we assemble the hack assembly code to complete the opeartion
static void translate_vm_function(TokenKind func, string label, int n)
{
    start_of_vm_func_command(func,label,n);

    //first we check if it is function or call
    if (func == tk_function)
    {
        //we create label.
        //label consist of class name + . + function name and offset
        output_assembler("(" + label + ")" );

        //As we use n arguments in the function, we push 0 ntimes into the stack
        cf = label;
        int i = 0;
        while (i < n)
        {
            output_assembler("@SP");
            output_assembler("A=M");
            output_assembler("M=0");
            //We assign 0 directly to the M register as having @0, did not pass the function 35 test
            output_assembler("@SP");
            output_assembler("AM=M+1");
            i++;
        }
    }
    else
    {
        cf = cf + "$rtrn";
        output_assembler("@" + cf + to_string(address));            //have the return address by concatenating the strings
        output_assembler("D=A");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");

        output_assembler("@LCL");                   //do the local push following the lecture slides psuedo code
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");

        output_assembler("@ARG");                   //do the argument push
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");

        output_assembler("@THIS");                  //do the this push
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");

        output_assembler("@THAT");                  //do the that push
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        output_assembler("@SP");
        output_assembler("M=M+1");

        output_assembler("@" + to_string(n));
        output_assembler("D=A");
        output_assembler("@5");
        output_assembler("D=D+A");
        output_assembler("@SP");
        output_assembler("D=M-D");
        output_assembler("@ARG");
        output_assembler("M=D");

        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");

        output_assembler("@" + label);                    //have a label that can return to the ideal address
        output_assembler("0;JMP");

        output_assembler("(" + cf + to_string(address) + ")");
        address++;
    }

    end_of_vm_command() ;
}

// translate vm push or pop command into assembly language
//we preapre switch statment and call hack assembly code to complete the operation
//we have first check whether push or pop and then chekc the local segments
static void translate_vm_stack(TokenKind stack,TokenKind segment,int offset)
{
    start_of_vm_stack_command(stack,segment,offset) ;
    //as we use the string to do the most of the operation, we prepare offset as string
    string var = to_string(offset);

    //this is the switch statement to check either the stack is pop or push
    switch(stack)
    {
        //when it is push, we check for the segment of the command
        case tk_push:

            //these if statements find the segment of the push
            //and have suitable hack assembly code for the suitable segment
            //when it is an static push

            if (segment == tk_static)
            {
                string address = cl + "." + var;
                //address line needs to have classname and the offset
                output_assembler("@" + address);
                output_assembler("D=M");
            }

            //when it is constant
            else if (segment == tk_constant)
            {
                output_assembler("@" + var );
                //the value of the offset is required
                output_assembler("D=A"); //you just call the @number
            }

            //when it is local, this, that and argument will have similar operation, but different segment name
            else if (segment == tk_local)
            {
                output_assembler("@LCL");
                //Move to the local segment
                output_assembler("D=M");
                output_assembler("@" + var);
                output_assembler("A=D+A");
                //move to the D+A address
                output_assembler("D=M");
            }

            //when it is THIS segment
            else if (segment == tk_this)
            {
                output_assembler("@THIS");              //move to the THIS pointer
                output_assembler("D=M");
                output_assembler("@" + var);
                output_assembler("A=D+A");
                output_assembler("D=M");
            }
            else if (segment == tk_that)
            {
                output_assembler("@THAT");              //holds the local
                output_assembler("D=M");
                output_assembler("@" + var);
                output_assembler("A=D+A");
                output_assembler("D=M");
            }
            else if (segment == tk_argument)
            {
                output_assembler("@ARG");               //holds the local
                output_assembler("D=M");
                output_assembler("@" + var);
                output_assembler("A=D+A");
                output_assembler("D=M");
            }

            else if (segment == tk_temp)
            {
                string temp = to_string(5 + offset);
                string reg = "R" + temp;
                output_assembler("@" + reg);            //holds the value of the this
                output_assembler("D=M");
            }
            else if (segment == tk_pointer)
            {
                string temp = to_string(3 + offset);
                string reg = "R" + temp;
                output_assembler("@" + reg);            //holds the value of the this
                output_assembler("D=M");
            }
            output_assembler("@SP");                    //the rest of the operation for the push will be the same
            output_assembler("A=M");
            output_assembler("M=D");                    //how you assign the variable and push it
            output_assembler("@SP");
            output_assembler("M=M+1");                  //move the pointer by incrementing one
            break;

        //Now we have pop
        case tk_pop:

            //there are two situations for pointer, either THAT or THIS
            //the use of THIS or THAT will depend on offset
            if(segment == tk_pointer)
            {
                output_assembler("@SP");
                output_assembler("AM=M-1");
                output_assembler("D=M");

                if (offset == 1) {                  //do that pointer
                    output_assembler("@THAT");
                }
                else if (offset == 0) {             //do this pointer
                    output_assembler("@THIS");
                }

                output_assembler("M=D");
            }
            else if (segment == tk_static) {
                string address = cl + "." + var;
                output_assembler("@SP");
                output_assembler("AM=M-1");
                output_assembler("D=M");
                output_assembler("@" + address);
                output_assembler("M=D");
            }
            else if (segment == tk_temp) {
                string temp = to_string(5 + offset);
                string reg = "R" + temp;
                output_assembler("@SP");
                output_assembler("AM=M-1");
                output_assembler("D=M");
                output_assembler("@" + reg);
                output_assembler("M=D");
            }

            //other semgents will have different first line code
            // but the rest of the code will be the same
            else if (segment == tk_argument || segment == tk_this || segment == tk_that || tk_local) {
                if (segment == tk_argument) {
                    output_assembler("@ARG");
                }
                else if (segment == tk_this) {
                    output_assembler("@THIS");
                }
                else if (segment == tk_that) {
                    output_assembler("@THAT");
                }
                else if (segment == tk_local) {
                    output_assembler("@LCL");
                }
                output_assembler("D=M");
                output_assembler("@" + var);
                output_assembler("D=D+A");
                output_assembler("@R13");
                output_assembler("M=D");
                output_assembler("@SP");
                output_assembler("AM=M-1");
                output_assembler("D=M");
                output_assembler("@R13");
                output_assembler("A=M");
                output_assembler("M=D");
            }

            break;

        default: ;
    }

    end_of_vm_command() ;
}


/************            AND HERE              **************/
///////////////////////////////////////////////////////////////

// grammar to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void parse_class() ;
static void parse_command() ;
static void parse_op() ;
static void parse_jump() ;
static void parse_func() ;
static void parse_stack() ;

// Note: always read one token after the one recognised

// vm_class ::= command* eoi
static void parse_class()
{
    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    while ( have(tk_vm_command) ) parse_command() ;
    mustbe(tk_eoi) ;

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;
}

// command ::= vm_operator | vm_jump | vm_func | vm_stack
static void parse_command()
{
    if ( have(tk_vm_operator) ) parse_op() ; else
    if ( have(tk_vm_jump) )     parse_jump() ; else
    if ( have(tk_vm_func) )     parse_func() ; else
    if ( have(tk_vm_stack) )    parse_stack() ; else
        mustbe(tk_vm_command) ;
}

// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
static void parse_op()
{
    TokenKind the_op = token_kind(mustbe(tk_vm_operator)) ;

    translate_vm_operator(the_op) ;
}

// vm_jump ::= jump label
// jump command - label, goto label or if-goto label
static void parse_jump()
{
    TokenKind command = token_kind(mustbe(tk_vm_jump)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;

    translate_vm_jump(command,label) ;
}

// vm_func ::= func label number
// call function-name #args or function function-name #locals
static void parse_func()
{
    TokenKind command = token_kind(mustbe(tk_vm_func)) ;
    string label = token_spelling(mustbe(tk_a_label)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    string delim = ".";
    cl = label.substr(0, label.find(delim));

    if (command == tk_function) {
        className = label;
        funName = label;                        //label.substr(label.find(delim), label.length() - 1);
    }

    translate_vm_function(command,label,number) ;
}

// vm_stack ::= stack segment number
// push segment offset or pop segment offset
static void parse_stack()
{
    TokenKind stack = token_kind(mustbe(tk_vm_stack)) ;
    TokenKind segment = token_kind(mustbe(tk_vm_segment)) ;
    int number = token_ivalue(mustbe(tk_a_number)) ;

    translate_vm_stack(stack,segment,number) ;
}

// main program
int main(int argc,char **argv)
{
    // initialise the tokeniser by reading the first token
    next_token() ;

    // parse a class and output VM code
    parse_class() ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
