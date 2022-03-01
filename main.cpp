#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
#include <iomanip>

using namespace std;
class Table
{
public:
    void insert(string s, int addr)
    {
        table.insert(pair<string, int>(s, addr));
    }
    int find(string s)
    {
        auto res = table.find(s);
        if (res == table.end())
        {
            return -1;
        }
        else
        {
            return res->second;
        }
    }
    const int operator[](string str){
      if(find(str)==-1)
        return -1;
      else
      return table[str];
    };
    void print(string str){
        cout<<str<<endl;
        cout<<"----------------------"<<endl;
        cout<<setw(8)<<"Symbol"<<setw(10)<<" Address"<<endl;
        for(auto it:table){
            cout<<setw(8)<<it.first<<setw(10)<<it.second<<endl;
        }
        cout<<"----------------------"<<endl;
    };
private:
    map<string, int> table;
};

class Line
{
public:
    Line(string s) : Text_(s){};
    string get_text() { return Text_; };
    void parsing();
    string label(){return label_;};
    string instruction(){return instruction_;};
    string operand(){return operand_;};
    void set_addr(int locctr){
      addr_ = locctr;
      flagAddr = true;
    };

    void set_opcode(int opcode){
      opcode_ = opcode;
    }
    int get_len(){
      return length_;
    }
    void set_len(int len){
      length_ = len;
    }
    void set_type(int type){
      type_ = type;
    }
    int get_type(){
      return type_;
    }
    int get_addr(){
      return addr_;
    }
    int get_machine_code(){
      return machine_code_;
    }
    //set machine code part

    void set_op();
    void set_n();
    void set_i();
    void set_x();
    void set_b();
    void set_p();
    void set_e();
    void set_disp(int disp);
    void set_machine_code(int mc);
    void set_r1(int r1);
    void set_r2(int r2);
    friend ostream& operator <<(ostream& os,const Line& l);
private:
    string Text_;
    int machine_code_ = 0;
    int opcode_;
    int addr_;
    int disp_;
    int length_ = 0;
    bool flagAddr = false;
    int type_ = -1;//0 : BYTE/WORD, 1: format 1, 2: format 2, 3: format 3,4: format 4,-1: this instr don't need to assemble
    string label_;
    string instruction_;
    string operand_;
};


Table OPTable;
Table SymbolTable;
Table AssemblyDirectTable;
enum class AssemblyDirect{
    START,
    BASE,
    BYTE,
    RESW,
    RESB,
    END
};
enum class ErrorCondition{
  DuplicateSymbol,
  InvalidOP,
  UndefinedSymbol,
};
void Line::set_op(){
  int temp;
  if(type_ == 1){
    temp = opcode_;
  }
  else if(type_ ==2){
    temp = opcode_ << 8;

  }
  else if(type_ == 3){
    temp = opcode_ << 16;
  }
  else if(type_ == 4){
    temp = opcode_ << 24;
  }
  else{
    cerr<<instruction_ <<" no need to assemble"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_n(){
  int temp;
  if(type_ == 3){
    temp = 1 << 17;
  }
  else if(type_ == 4){
    temp = 1 << 25;
  }
  else{
    cerr<<instruction_<<" don't have n field"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_i(){
  int temp;
  if(type_ == 3){
    temp = 1 << 16;
  }
  else if(type_ == 4){
    temp = 1 << 24;
  }
  else{
    cerr<<instruction_<<" don't have i field"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_x(){
  int temp;
  if(type_ == 3){
    temp = 1 << 15;
  }
  else if(type_ == 4){
    temp = 1 << 23;
  }
  else{
    cerr<<instruction_<<" don't have x field"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_b(){
  int temp;
  if(type_ == 3){
    temp = 1 << 14;
  }
  else if(type_ == 4){
    temp = 1 << 22;
  }
  else{
    cerr<<instruction_<<" don't have b field"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_p(){
  int temp;
  if(type_ == 3){
    temp = (1 << 13);
  }
  else if(type_ == 4){
    temp = 1 << 21;
  }
  else{
    cerr<<instruction_<<" don't have p field"<<endl;
  }

  machine_code_ |= temp;

}
void Line::set_e(){
  int temp;
  if(type_ == 3){
    temp = 1 << 12;
  }
  else if(type_ == 4){
    temp = 1 << 20;
  }
  else{
    cerr<<instruction_<<" don't have e field"<<endl;
  }
  machine_code_ |= temp;
}
void Line::set_disp(int disp){
  //calculate target address and fill in machine code
  disp_ = disp;
  if(type_ == 3){
    disp &= 4095;
  }
  else if(type_ == 4){
    disp &= 1048575;
  }
  machine_code_ |= disp;
}
void Line::set_machine_code(int mc){
  machine_code_ = mc;
}
void Line::set_r1(int r1){
  machine_code_ |= (r1<<4);
}
void Line::set_r2(int r2){
  machine_code_ |= r2;
}
void init(){
    OPTable.insert("STL",0x14);
    OPTable.insert("LDB",0x68);
    OPTable.insert("JSUB",0x48);
    OPTable.insert("LDA",0x00);
    OPTable.insert("COMP",0x28);
    OPTable.insert("JEQ",0x30);
    OPTable.insert("J",0x3C);
    OPTable.insert("STA",0x0C);
    OPTable.insert("CLEAR",0xB4);//2
    OPTable.insert("LDT",0x74);
    OPTable.insert("TD",0xE0);
    OPTable.insert("RD",0xD8);
    OPTable.insert("COMPR",0xA0); //2
    OPTable.insert("STCH",0x54);
    OPTable.insert("TIXR",0xB8);//2
    OPTable.insert("JLT",0x38);
    OPTable.insert("STX",0x10);
    OPTable.insert("RSUB",0x4C);
    OPTable.insert("LDCH",0x50);
    OPTable.insert("WD",0xDC);

    AssemblyDirectTable.insert("START",(int)AssemblyDirect::START);
    AssemblyDirectTable.insert("BASE",(int)AssemblyDirect::BASE);
    AssemblyDirectTable.insert("BYTE",(int)AssemblyDirect::BYTE);
    AssemblyDirectTable.insert("BYTE",(int)AssemblyDirect::BYTE);
    AssemblyDirectTable.insert("RESW",(int)AssemblyDirect::RESW);
    AssemblyDirectTable.insert("RESB",(int)AssemblyDirect::RESB);
    AssemblyDirectTable.insert("END",(int)AssemblyDirect::END);
}
void Line::parsing(){
  string temp = Text_;
  size_t pos;
  bool flag = false;
  while ((pos = temp.find_first_of("\t ")) != string::npos)
    {
        string token;

        token = temp.substr(0, pos);
        if(OPTable.find(token)!=-1 || AssemblyDirectTable.find(token)!=-1 || token[0] =='+'){
          instruction_ = token;
          flag = true;
        }
        else if(flag==false){
          label_ = token;
        }
        else{
          operand_ = token;
        }
        temp.erase(0, pos + 1);
    }
    if(flag == true){
      operand_ = temp;
    }
    else{
       if(OPTable.find(temp)!=-1 || AssemblyDirectTable.find(temp)!=-1 || temp[0] =='+'){
          instruction_ = temp;
          flag = true;
        }
    }

}
void set_error_flag(int8_t& error,ErrorCondition ec){
  switch(ec){
    case ErrorCondition::DuplicateSymbol:
      error |= 1;
      break;
    case ErrorCondition::InvalidOP:
      error |= 2;
      break;
    case ErrorCondition::UndefinedSymbol:
      error |= 4;
      break;
  }
}
ostream& operator <<(ostream& os,const Line& l){
  if(l.flagAddr)
    os<<"label: "<<l.label_<<" instruction: "<<l.instruction_<<" operand: "<<l.operand_<<" address: "<<hex<<l.addr_<<" type: "<<dec<<l.type_<<" len: "<<l.length_<<" machine_code:  "<<hex<<l.machine_code_<<endl;
  else{
    os<<"label: "<<l.label_<<" instruction: "<<l.instruction_<<" operand: "<<l.operand_<<endl;
  }
  return os;
}
bool isNumber(string s){
  for(auto c:s){
    if(isdigit(c)==0) return false;
  }
  return true;
}
void pass1(int& locctr,int& starting_addr,int& file_length,int& base,vector<Line>& instructions,int8_t& error){
  if(instructions[0].instruction()== "START"){
    //cout<<"this is start"<<endl;
    //cout<<instructions[0].operand()<<endl;
    stringstream ss;
    ss<<instructions[0].operand();
    ss>>hex>>locctr;
    starting_addr = locctr;
    //cout<<"locctr = "<<hex<<locctr<<endl;
    instructions[0].set_addr(locctr);
  }
  else{
    locctr = 0;
    starting_addr = 0;
  }
  for(int i = 1;i<instructions.size();i++){
    instructions[i].set_addr(locctr);
    string label = instructions[i].label();
    string instruction = instructions[i].instruction();
    string operand = instructions[i].operand();
    if(label != ""){
      cout<<"label "<<label<<" insert to symbol table"<<endl;
      if(SymbolTable.find(label)!=-1){
        cerr<<"label "<<label<<" duplicate"<<endl;
        set_error_flag(error,ErrorCondition::DuplicateSymbol);
      }
      else{
        SymbolTable.insert(label,locctr);
      }
    }
    if(OPTable.find(instruction)!=-1){
      if(instruction=="TIXR"||instruction=="CLEAR"||instruction=="COMPR"){
        //type 2
        instructions[i].set_type(2);
        instructions[i].set_len(2);
        locctr += 2;
      }
      else{
        instructions[i].set_type(3);
        instructions[i].set_len(3);
        locctr += 3;
      }
    }
    else if(instruction[0] == '+'){
      //type 4;
      if(OPTable.find(instruction.substr(1,instruction.size()))!=-1){
        instructions[i].set_type(4);
        instructions[i].set_len(4);
        locctr += 4;
      }
      else{
        set_error_flag(error,ErrorCondition::InvalidOP);
      }
    }
    else if(instruction == "BYTE"){
      instructions[i].set_type(0);
      if(operand[0] == 'C'){
        char str[10];
        sscanf(operand.c_str(),"C\'%[^\']",str);
        if(str[0] =='\0'){
            cerr<<"str is empty"<<endl;
        }
        string real(str);
        locctr+= real.size();
        if(real.size() <= 3 && real.size()>0){
          instructions[i].set_len(real.size());
        }
        else{
          cerr<<"byte operand can't over 3 bytes."<<endl;
        }
      }
      else{
        locctr += 1;
        instructions[i].set_len(1);
      }

    }
    else if(instruction == "WORD"){
      instructions[i].set_type(0);
      instructions[i].set_len(3);
      locctr += 3;
    }
    else if(instruction == "RESB"){
      int num;
      stringstream ss;
      ss<< operand;
      ss>>num;
      locctr += num;
    }
    else if(instruction == "RESW"){
      int num;
      stringstream ss;
      ss<< operand;
      ss>>num;
      locctr += num*3;
    }
    else if(instruction == "BASE"){
      instructions[i].set_type(0);
      ;
    }
    else if(instruction == "END"){
      break;
    }
    else{
      cout<<"invalid opcode"<<instruction<<endl;
      set_error_flag(error,ErrorCondition::InvalidOP);
    }
  }
  file_length = locctr - starting_addr;
}

void pass2(vector<Line>& instructions,int starting_addr,int file_length,int8_t& error,int& base){
  fstream file("output.out",ios::out|ios::trunc);
  if(instructions[0].instruction() == "START"){
    //char filename[7];
    string filename = instructions[0].label();
    //cout<<"starting addr: "<<starting_addr<<endl;
    //cout<<"file length"<<file_length<<endl;
    file<<"H";
    file<<left<<setw(6)<<filename;
    file<<hex<<right<<setw(6)<<setfill('0')<<starting_addr;
    file<<hex<<right<<setw(6)<<setfill('0')<<file_length;
    file<<endl;
  }
  bool base_set = false;
  int inOP;
  int i = 0;
  int A_reg;
  int S_reg;
  int X_reg;
  int T_reg;
  string instruction = instructions[0].instruction();
  int text_address = starting_addr;
  int text_length = 0;
  stringstream ss;
  stringstream mss;
  bool flag = false;
  while(i<instructions.size() && instruction != "END"){
    string operand = instructions[i].operand();


    if(instructions[i].get_type() == 2){
      instructions[i].set_opcode(OPTable[instruction]);
      instructions[i].set_op();
      if(instruction == "CLEAR"){
        int r1_value;
        if(operand == "X"){
          r1_value = 1;
          X_reg = 0;
        }
        else if(operand == "S"){
          r1_value = 4;
          S_reg = 0;
        }
        else if(operand == "A"){
          r1_value = 0;
          A_reg = 0;
        }
        else if(operand == "T"){
          r1_value = 5;
          T_reg = 0;
        }
        else{
          r1_value = -1;
        }
        instructions[i].set_r1(r1_value);
      }
      else if(instruction == "TIXR"){
        int r1_value;
        if(operand == "X"){
          r1_value = 1;
          X_reg += 1;
        }
        else if(operand == "S"){
          r1_value = 4;
          S_reg += 1;
        }
        else if(operand == "A"){
          r1_value = 0;
          A_reg += 1;
        }
        else if(operand == "T"){
          r1_value = 5;
          T_reg += 1;
        }
        else{
          r1_value = -1;
        }
        instructions[i].set_r1(r1_value);
      }
      else if(instruction == "COMPR"){
        int r1_value = -1;
        int r2_value = -1;
        char op1,op2;
        sscanf(operand.c_str(),"%c,%c",&op1,&op2);
        if(op1 == 'A'){
          r1_value = 0;
        }
        else if(op1 == 'S'){
          r1_value = 4;
        }
        else if(op1 == 'T'){
          r1_value = 5;
        }
        else if(op1 == 'X'){
          r1_value = 1;
        }

        if(op2 == 'A'){
          r2_value = 0;
        }
        else if(op2 == 'S'){
          r2_value = 4;
        }
        else if(op2 == 'T'){
          r2_value = 5;
        }
        else if(op2 == 'X'){
          r2_value = 1;
        }
        instructions[i].set_r1(r1_value);
        instructions[i].set_r2(r2_value);
      }
    }
    else if(instructions[i].get_type()==3){
      instructions[i].set_opcode(OPTable.find(instruction));
      instructions[i].set_op();
      if(operand[0] == '#' || operand[0]=='@'){
        if(operand[0] == '#'){
          instructions[i].set_i();
        }
        else if(operand[0] == '@'){
          instructions[i].set_n();
        }
        string realOperand = operand.substr(1,operand.size());
        inOP = SymbolTable.find(operand.substr(1,operand.size()));
        if(inOP == -1){
          int realinSym = -1;
          if(realOperand.size()>2){ //如 instr realOperand,x 要把realOperand抽出來
            realinSym = SymbolTable.find(realOperand.substr(0,realOperand.size()-2));
          }
          if(isNumber(realOperand)){
            //cout << realOperand<<" Operand is a number." << endl;
            stringstream ss ;
            int disp;
            ss<<realOperand;
            ss>>disp;
            instructions[i].set_disp(disp);
          }
          else if(realinSym != -1){
            //cout<<realOperand<<endl;
            //cout<<"real operand in symbol table"<<endl;
          }
          else{
            cerr << instruction <<"'s operand "<<operand<<" not in SymbolTable."<<endl;
          }
        }
        else{

          int target_address = SymbolTable[realOperand];
          int p_diff = target_address - (instructions[i].get_addr() + instructions[i].get_len());
          int b_diff = target_address - base;
          if(-2048 <= p_diff && p_diff <= 2047){
            instructions[i].set_p();
            instructions[i].set_disp(p_diff);
          }
          else if(base_set == true && 0 <= b_diff && b_diff <= 4095){
            instructions[i].set_b();
            instructions[i].set_disp(b_diff);
          }

        }
      }
      else{ // is not immediate or indirect type 3
        instructions[i].set_i();
        instructions[i].set_n();
        inOP = SymbolTable.find(operand);
        if(inOP == -1){
          int realinSym = -1;
          if(operand.size()>2){
            realinSym = SymbolTable.find(operand.substr(0,operand.size()-2));
          }
          if(isNumber(operand)){
            //cerr << "Operand is a number." << endl;
          }
          else if(realinSym!=-1){

            instructions[i].set_x();
            int target_address = SymbolTable[operand.substr(0,operand.size()-2)];
            int p_diff = target_address - (X_reg + instructions[i].get_addr() + instructions[i].get_len());
            int b_diff = target_address -(base + X_reg);
            if(-2048 <= p_diff && p_diff <= 2047){
              instructions[i].set_p();
              instructions[i].set_disp(p_diff);
            }
            else if(base_set == true && 0 <= b_diff && b_diff <= 4095){
              instructions[i].set_b();
              instructions[i].set_disp(b_diff);
            }
            else{
              cerr<<instruction<<" can neither pc nor base."<<endl;
            }
          }
          else{
            cerr << instruction <<"'s operand "<<operand<<" not in SymbolTable."<<endl;
          }
        }
        else{
          int target_address = SymbolTable[operand];
          int p_diff = target_address - (instructions[i].get_addr() + instructions[i].get_len());
          int b_diff = target_address - base;
          if(-2048 <= p_diff && p_diff <= 2047){
            instructions[i].set_p();
            instructions[i].set_disp(p_diff);
          }
          else if(base_set == true && 0 <= b_diff && b_diff <= 4095){
            instructions[i].set_b();
            instructions[i].set_disp(b_diff);
          }
          else{
            cerr<<instruction<<" can neither pc nor base."<<endl;
          }
        }
      }
    }
    else if(instructions[i].get_type()==4){
      instructions[i].set_opcode(OPTable.find(instruction.substr(1,instruction.size())));
      instructions[i].set_op();
      instructions[i].set_e();
      if(operand[0] == '#' || operand[0]=='@'){ // immediate or indirect
        if(operand[0] == '#'){
          instructions[i].set_i();
        }
        else if (operand[0] == '@'){
          instructions[i].set_n();
        }
        string realOperand = operand.substr(1,operand.size());
        inOP = SymbolTable.find(realOperand);
        if(inOP == -1){
          if(isNumber(realOperand)){
            //cout<<instruction<<"'s operand is a number"<<endl;
            stringstream ss ;
            int disp;
            ss<<realOperand;
            ss>>disp;
            instructions[i].set_disp(disp);
          }
          else{
            cerr<<instruction<<"'s operand is not in symbol table"<<endl;
          }
        }
        else{
          int target_address = SymbolTable[realOperand];
          instructions[i].set_disp(target_address);
        }
      }
      else{ //simple
        int target_address = SymbolTable[operand];
        instructions[i].set_i();
        instructions[i].set_n();
        instructions[i].set_disp(target_address);
        mss<<"M"<<hex<<setfill('0')<<setw(6)<<instructions[i].get_addr()+1<<setfill('0')<<setw(2)<<5<<endl;
      }

    }
    else if(instructions[i].get_type()==0){
      if(instruction =="BASE"){
        //cout<< " Find BASE!!!"<<endl;
        base = SymbolTable[operand];
        base_set = true;
      }
      else if(instruction =="BYTE"){
        //cout<<"byte type "<<operand<<endl;
        if(operand[0]=='C'){
          int val = 0;
          char buffer[10];
          sscanf(operand.c_str(),"C\'%[^\']",buffer);
          if(buffer[0] =='\0'){
            cerr<<"buffer is empty"<<endl;
          }
          string str(buffer);
          //cout<<"byte operand "<<str<<endl;
          for(int j = 0;j<str.size();j++){
              //cout<<"str["<<j<<"] "<<str[j]<<endl;
              //cout<<str[j]-'A'<<endl;
              val |= (int)(str[j]);
              if(j!=str.size()-1){
                val = val << 8;
              }
          }
          //cout<<hex<<val<<endl;
          instructions[i].set_machine_code(val);
        }
        else if(operand[0]=='X'){
          char str[10];
          sscanf(operand.c_str(),"X\'%[^\']",str);
          stringstream ss;
          ss << str;
          int val;
          ss >> std::hex>>val;
          //cout<<"num = "<<val<<endl;
          instructions[i].set_machine_code(val);
        }
      }
    }
    else{
      //
    }
    if(instruction == "RESW" || instruction == "RESB"){

      if(flag){

        file<<"T"<<setw(6)<<setfill('0')<<hex<<text_address<<setw(2)<<hex<<setfill('0')<<text_length<<ss.str()<<endl;
        ss.clear();
        ss.str("");
        flag = false;
      }
    }
    else if(ss.str().size() + (instructions[i].get_len()*2) <= 60){
      if(flag == false){
        flag = true;
        text_length = 0;
        text_address = instructions[i].get_addr();
      }
      ss<<setw(instructions[i].get_len()*2)<<setfill('0')<<hex<<instructions[i].get_machine_code();
      text_length += instructions[i].get_len();
    }
    else{
      file<<"T"<<setw(6)<<setfill('0')<<hex<<text_address<<setw(2)<<hex<<setfill('0')<<text_length<<ss.str()<<endl;
      ss.clear();
      ss.str("");
      text_length = instructions[i].get_len();
      text_address = instructions[i].get_addr();
      ss<<setw(instructions[i].get_len()*2)<<hex<<instructions[i].get_machine_code();
    }



    i++;
    instruction = instructions[i].instruction();
  }
  file<<"T"<<setw(6)<<setfill('0')<<hex<<text_address<<setw(2)<<hex<<setfill('0')<<text_length<<ss.str()<<endl;
  file<<mss.str();
  file<<"E"<<setw(6)<<hex<<setfill('0')<<starting_addr<<endl;
  file.close();
}




void print_error_message(int8_t ec){

  if(ec&1){
    cerr<<"Duplicate symbol define"<<endl;
  }
  else if(ec&2){
    cerr<<"Invalid opcode"<<endl;
  }
  else if(ec&4){
    cerr<<"Undefine symbol"<<endl;
  }
  else{
    cout<<"Well define!!"<<endl;
  }
}

int main()
{
    //read instruction and create Line and assign address to every Line
    int locctr = 0; //SIC/XE memory addr 最大2^20 int 足夠
    int starting_addr = 0;
    int file_length;
    int base;    // for base register
    int8_t error = 0;
    init();
    //read all instruction
    vector<Line> instructions;
    string filename;
    cout<<"Please enter input file name"<<endl;
    cin>>filename;
    ifstream file(filename);
    string line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            instructions.emplace_back(Line(line));
        }
        file.close();
    }
    else
    {
        cerr << "Can't open the file" << endl;
    }
    //test read file function
    int cnt = 1;

    for(auto &it:instructions){
      it.parsing();
      //cout<<cnt<<" label: "<<it.label()<<" instruction: "<<it.instruction()<<" operand: "<<it.operand()<<endl;
      cnt++;
    }
    // pass 1;
    pass1(locctr,starting_addr,file_length,base,instructions,error);
    print_error_message(error);
    /*
    cout<<"After Pass1 ========"<<endl<<endl;
    cnt = 1;
    for(auto &it:instructions){
      cout<< dec<<cnt <<" "<<it;
      cnt++;
    }
    */
    pass2(instructions,starting_addr,file_length,error,base);
    cout<<endl;
    SymbolTable.print("SystemTable");
    cout<<endl<<"Process finish!!"<<endl;
    cout<<"Output file in output.out."<<endl;
    system("pause");
    return 0;
    // cout<<"finish process"<<endl;
    /*
    cout<<"After Pass2 ========"<<endl<<endl;
    cnt = 1;
    for(auto &it:instructions){
      cout<< dec<<cnt <<" "<<it;
      cnt++;
    }
    */

}
