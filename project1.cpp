#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

int error_count = 0;
bool follows_BNF = true;

/*------------------------------------------------------------------------------------------------------------------------------*/

//Recursive Descending Parsing

//checks if keyword is valid
bool keyword(string key){
    if(key == "float" || key == "void"){
        return true;
    }
    else{
        error_count += 1;
        if(key != ")" && key != "(" && key != "{"){
            cout<<"Error "<<error_count<<":"<<" Valid keyword expected! -> "<<"'"<<key<<"' not expected"<<endl;
        }
        else{
            cout<<"Error "<<error_count<<":"<<" Valid keyword expected as parameter!"<<endl;
        }
        follows_BNF = false;
        return false;
    }
    return false;
}

//checks for a valid identifier
bool ident(string ident, map<string, string> m){
    if(m.find(ident) != m.end()){ //anything not in tokenMap
        error_count += 1;
        if(ident != ")" && ident != "(" && ident != "{"){
            cout<<"Error "<<error_count<<":"<<" Valid identifier expected! -> "<<"'"<<ident<<"' not expected"<<endl;
        }
        else{
            cout<<"Error "<<error_count<<":"<<" Valid identifier expected as parameter!"<<endl;
        }
        follows_BNF = false;
        return false;
    }
    else{
        return true;
    }
}

//checks if declarations are valid
bool declares(string key, string identifier, map<string, string> m){

    //calls keyword and ident functions for valid declarations
    if(keyword(key) && ident(identifier, m)){
        return true;
    }
    else{
        follows_BNF = false;
        false;
    }
    return false;
}

//checks if expression is valid
bool expr(vector<string> l, int i, map<string, string> m){
    if(!(m.find(l[i]) != m.end()) && l[i+1] == ";"){
        return true;
    }
    //sets expression to allow ident {+|-} ident (<expr>)
    else if(!(m.find(l[i]) != m.end()) && (l[i+1] == "+" || l[i+1] == "-"))
    {
        return expr(l, i+2, m); //recursive call for next index
    }
    else{//check for invalid expression or missing semicolon
        if(l[i+1] != ";")
        {
            error_count += 1;
            cout<<"Error "<<error_count<<":"<< " Syntax - semicolon expected after indentifier at end of expression! -> "<<"'"<<l[i]<<"'"<<endl;
            follows_BNF = false;
            return false;
        }
        else{
            error_count += 1;
            cout<<"Error "<<error_count<<":"<< " Invalid expression! -> "<<"'"<<l[i]<<" "<<l[i+1]<<"'"<<endl;
            follows_BNF = false;
            return false;
        }
    }
    return false;
}


void parserProgram(vector<string> l, map<string, string> m){//l -> lexeme_list, m -> tokenMap

    //BNF & Syntax Errors displayed in order; BNF errors displayed one at a time, treated as compile time errors
    //if 1st BNF error fixed, the next BNF error, if available, will display at compile time - priority through Top-Down analysis
    if(keyword(l[0]) && ident(l[1],m)){
        if(l[2] == "("){
            if(keyword(l[3]) && ident(l[4], m) && l[5] == ")" && l[6] == "{"){ //calls keyword & ident functions
                for(int i = 7; i < l.size(); i++){
                    if(l[i] == "float" || l[i] == "void"){ //if next lexeme in function body is valid keyword, call <declares>
                        if(declares(l[i], l[i+1], m)){
                            if(l[i+2] != ";"){ //checks for missing semicolon in declaration
                                error_count += 1;
                                cout<<"Error "<<error_count<<":"<<" Syntax - semicolon expected after indentifier in declaration! -> "<<l[i]<<" "<<l[i+1]<<"'"<<endl;
                                follows_BNF = false;
                            }
                        }
                        
                        i += 1; 
                    }
                    
                    if(!(m.find(l[i]) != m.end()) && !(m.find(l[i+1]) != m.end()) && l[i-1] == ";" || l[i-1] == "{"){ //checks invalid keywords of declaration
                        error_count += 1;
                        cout<<"Error "<<error_count<<":"<<" Invalid declaration for BNF! -> "<<"'"<<l[i]<<"' not expected"<<endl;
                        follows_BNF = false;
                        break;
                    }
                    
                    
                    if(!(m.find(l[i]) != m.end()) && l[i+1] == "="){ //if next lexeme in function body has assignment call <expr> function
                        if(expr(l, i+2, m)){}
                    }   
                    
                }
            }
            else{
                //check for missing parenthesis or braces syntax errors
                int isParenthesis = 0;
                int isBraces = 0;
                for(int i = 0; i<10;i++){
                    if(l[i] == ")"){
                        isParenthesis = 1;
                    }
                    if(l[i] == "{"){
                        isBraces = 1;
                    }
                }
                if(isParenthesis == 0){
                        follows_BNF = false;
                        error_count += 1;
                        cout<<"Error "<<error_count<<":"<<" Syntax - Missing right parenthesis for function parameters!"<<endl;
                }
                if(isBraces == 0){
                        follows_BNF = false;
                        error_count += 1;
                        cout<<"Error "<<error_count<<":"<<" Syntax - Missing function opening left curly braces!"<<endl;
                }
            }
        }
        else{
            follows_BNF = false;
            error_count += 1;
            cout<<"Error "<<error_count<<":"<<" Syntax - Missing left parenthesis for function!"<<endl;
        }
    }
    else{
        cout<<"Implicit declaration of function for BNF"<<endl;
    }

    //check for closing function braces
    if(l.back() == "}"){}
    else{
        follows_BNF = false;
        error_count += 1;
        cout<<"Error "<<error_count<<":"<<" Syntax - Missing function closing right curly braces!"<<endl;
    }

    //checks for errors, if none then bnf is true
    if(follows_BNF){
        cout<<"The try program is generated by BNF grammar for Try Program"<<endl;
    }

    
}


/*------------------------------------------------------------------------------------------------------------------------------*/

//Lexical Analysis Display
vector<string> lexicalAnalyzer(string filename, map<string, string> tokenMap){
    
    ifstream myfile(filename);
    string char_string;
    string token;
    string line;
    string lexeme;
    vector<string> lexeme_list;
    
    if (myfile.is_open())
    {
        while(getline(myfile, line))
        {
            //while the file is open, append each character from each line of input into the char_string
        	for(int i = 0; i < line.length(); i++)
        	{
                if(isalpha(line[i])){
                    char_string += line[i];
                }
                //if the character is not a letter, then add space to allow seperation between symbols and words to be stored in the char_string
                else if(!isalpha(line[i])){ 
                    char_string += ' ';
                    char_string += line[i];
                    char_string += ' ';
			    }
            }
        }

        myfile.close();
    }

    //converts the characters (words & symbols) of the char_string into strings; pushed into a lexeme list for indexing
    stringstream ss(char_string);
    while(ss >> lexeme){
        lexeme_list.push_back(lexeme);
    }
    
    //indexing the lexeme_list and tokenMap to assign tokens; displaying both lexemes and tokens as output
    for(const string& lexeme : lexeme_list){
        if(tokenMap.find(lexeme) != tokenMap.end()){
            token = tokenMap[lexeme];
        }
        else{
            token = "identifier";
        }
        cout<<"Lexeme: " + lexeme << "       Token: " + token <<endl;
    }

    return lexeme_list;
}


int main(){
    
    string filename;

    //assigning keys (lexemes) and values (tokens) to the string map; used for iteration of the input
    map<string, string> tokenMap;
    tokenMap[";"] = "semicolon"; tokenMap["float"] = "keyword";
    tokenMap["}"] = "right_curlybrace"; tokenMap["int"] = "keyword";
    tokenMap["{"] = "left_curlybrace"; tokenMap["string"] = "keyword";
    tokenMap[")"] = "right_parenthesis"; tokenMap["bool"] = "keyword";
    tokenMap["("] = "left_parenthesis"; tokenMap["char"] = "keyword"; tokenMap["="] = "equals";
    tokenMap["+"] = "add_op";  tokenMap["double"] = "keyword"; tokenMap[","] = "comma";
    tokenMap["-"] = "sub_op";  tokenMap["return"] = "keyword"; tokenMap[":"] = "colon";
    tokenMap["*"] = "mult_op"; tokenMap["void"] = "keyword"; tokenMap["/"] = "div_op";

    //example -> C:\Users\Jason\Downloads\cpp\project.txt
    //can enter filename as well depending on the IDE and if the file is stored in same directory
    cout<<"Enter Filepath or Filename: ";
    cin>>filename;
    cout<<"\n"<<endl;
    
    //calls lexical analyzer function to display lexeme and tokens; returns a list of lexemes
    vector<string> list = lexicalAnalyzer(filename, tokenMap);

    cout<<"\n"<<endl;
    
    //calls program for BNF syntax analysis and recursive Top-Down parsing
    parserProgram(list, tokenMap);

    cout<<"\n"<<endl;

    return 0;
}